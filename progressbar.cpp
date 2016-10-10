#include "progressbar.h"

ProgressBar::ProgressBar() :
	initialized( false ),
	isRunning( false ),
	totalWeight( 0 ),
	thread( nullptr ),
	activeSegment( 0 ),
	segments( 0 ),
	segmentProgresses( 0 ),
	segmentStartTimes( 0 ),
	displaySubProgress( false ) {}

ProgressBar::ProgressBar( const std::vector<Segment>& segments, bool displaySubProgress ) : ProgressBar() {
	init( segments, displaySubProgress );
}

ProgressBar::ProgressBar( const std::vector<Segment>& segments, extraDataFunc extraDataGenerator, bool displaySubProgress ) : ProgressBar() {
	init( segments, extraDataGenerator, displaySubProgress );
}

ProgressBar::~ProgressBar() {
	finish( true );
}

winsize ProgressBar::getConsoleSize() {
	struct winsize size;
	ioctl( STDOUT_FILENO, TIOCGWINSZ, &size );

	return size;
}

unsigned short ProgressBar::getConsoleHeight() {
	return getConsoleSize().ws_row;
}

unsigned short ProgressBar::getConsoleWidth() {
	return getConsoleSize().ws_col;
}

void ProgressBar::init( const std::vector<Segment> & segments, bool displaySubProgress ) {
	init( segments, extraDataFunc(), displaySubProgress );
}

void ProgressBar::init( const std::vector<Segment> & segments, extraDataFunc extraDataGenerator, bool displaySubProgress ) {
	if ( initialized )
		// Should I throw an exception?
		return;

	this->segments.reserve( segments.size() );
	segmentProgresses.resize( segments.size(), 0.0 );
	segmentStartTimes.reserve( segments.size() );

	for ( const Segment & segment : segments ) {
		this->segments.push_back( segment );

		totalWeight += segment.getWeight();
	}

	this->extraDataGenerator = extraDataGenerator;
	this->displaySubProgress = displaySubProgress;

	initialized = true;
}

void ProgressBar::start() {
	std::cout << "\33[?25l";

	startTime = HRC::now();
	segmentStartTimes[0] = HRC::now();
	isRunning = true;
	thread = std::unique_ptr<std::thread>( new std::thread( &ProgressBar::renderThread, this ) );

	auto handle = thread->native_handle();
	pthread_setname_np( handle, "ProgressBar" );
}

void ProgressBar::finish( bool blocking ) {
	if ( !initialized || !isRunning )
		return;

	isRunning = false;

	if ( blocking && thread->joinable() )
		thread->join();

	// Make sure the bar is filled
	renderBar( 1.0 );

	std::cout << "\33[?25h\n\n\n" << (displaySubProgress ? "\n\n\n" : "") << std::flush;
}

void ProgressBar::updateProgress( size_t numSegment, double progress ) {
	if ( !initialized )
		return;

	if ( activeSegment != numSegment ) {
		activeSegment = numSegment;
		segmentStartTimes[activeSegment] = HRC::now();
	}

	scoped_lock lock( segmentsMutex );

	segmentProgresses[numSegment] = std::max( 0.0, std::min( 1.0, progress ) );
}

void ProgressBar::updateProgress( size_t numSegment, size_t workDone, size_t workToDo ) {
	updateProgress( numSegment, div( workDone, workToDo ) );
}

int ProgressBar::getNumSegments() {
	scoped_lock lock( segmentsMutex );

	return segmentProgresses.size();
}

template<typename T>
inline double ProgressBar::div( const T & lhs, const T & rhs ) {
	return static_cast<double>(lhs) / static_cast<double>(rhs);
}

std::string ProgressBar::getPercentString( double progress, size_t width ) {
	std::stringstream sstr;

	sstr << std::setw( 5 ) << std::fixed << std::setprecision( 1 ) << progress * 100.0 << '%';

	return centerString( width, sstr.str() );
}

std::string ProgressBar::centerString( size_t width, const std::string& str ) {
	size_t len = str.length();

	if ( width < len ) {
		return str;
	}

	int diff = width - len;
	int pad1 = diff / 2;
	int pad2 = diff - pad1;

	return std::string( pad1, ' ' ) + str + std::string( pad2, ' ' );
}

void ProgressBar::renderThread() {
	double progress;

	while ( isRunning ) {
		std::this_thread::sleep_for( std::chrono::milliseconds( defaultTimeout ) );

		progress = getTotalProgress();

		if ( progress == 1.0 )
			return;

		renderBar( progress );
	}
}

void ProgressBar::renderBar( double progress ) {
	const duration timeElapsed = std::chrono::duration_cast<duration>(HRC::now() - startTime);
	const duration timeRemaining = (1.0 / progress - 1.0) * timeElapsed;
	const size_t barWidth = getConsoleWidth();
	const size_t splitPos = barWidth * progress;
	const std::string percentString = getPercentString( progress, barWidth );

	std::cout << "\33[s\33[K" << centerString( barWidth, "===== " + (displaySubProgress ? "Total" : getActiveSegment().getTitle()) + " =====" ) << '\n';
	std::cout << "\33[K\33[7m" << percentString.substr( 0, splitPos ) << "\33[0m" << percentString.substr( splitPos ) << '\n';
	std::cout << "\33[KTime elapsed: " << timeElapsed << "\tTime remaining: " << timeRemaining;

	if ( extraDataGenerator )
		std::cout << "\t" << extraDataGenerator( progress );

	if ( displaySubProgress ) {
		const Segment & segment = getActiveSegment();
		const double segmentProgress = segmentProgresses[activeSegment];
		const duration segmentTimeElapsed = std::chrono::duration_cast<duration>(HRC::now() - segmentStartTimes[activeSegment]);
		const duration segmentTimeRemaining = (1.0 / segmentProgress - 1.0) * timeElapsed;
		const size_t segmentSplitPos = barWidth * segmentProgress;
		const std::string segmentPercentString = getPercentString( segmentProgress, barWidth );

		std::cout << "\n\33[K" << centerString( barWidth, "===== " + segment.getTitle() + " =====" ) << '\n';
		std::cout << "\33[K\33[7m" << segmentPercentString.substr( 0, segmentSplitPos ) << "\33[0m" << segmentPercentString.substr( segmentSplitPos ) << '\n';
		std::cout << "\33[KTime elapsed: " << segmentTimeElapsed << "\tTime remaining: " << segmentTimeRemaining;

		if ( segment.hasExtraDataGenerator() )
			std::cout << "\t" << segment.getExtraDataGenerator()(progress);
	}

	std::cout << "\33[u" << std::flush;
}

double ProgressBar::getTotalProgress() {
	double progress = 0.0;
	scoped_lock lock( segmentsMutex );

	for ( size_t i = 0; i < segmentProgresses.size(); i++ ) {
		progress += div( segments[i].getWeight(), totalWeight ) * segmentProgresses[i];
	}

	return progress;
}

const ProgressBar::Segment & ProgressBar::getActiveSegment() {
	scoped_lock lock( segmentsMutex );

	return segments[activeSegment];
}

std::ostream & operator<<( std::ostream & os, ProgressBar::duration dSeconds ) {
	typedef std::chrono::duration<int, std::ratio<24 * 3600>> days;
	using std::chrono::hours;
	using std::chrono::minutes;
	using std::chrono::duration_cast;

	days dDays = duration_cast<days>(dSeconds);
	dSeconds -= duration_cast<ProgressBar::duration>(dDays);

	hours dHours = duration_cast<hours>(dSeconds);
	dSeconds -= duration_cast<ProgressBar::duration>(dHours);

	minutes dMinutes = duration_cast<minutes>(dSeconds);
	dSeconds -= duration_cast<ProgressBar::duration>(dMinutes);

	if ( dDays.count() > 0 )
		os << std::fixed << std::setw( 2 ) << dDays.count() << 'd';

	os << std::fixed << std::setw( 2 ) << std::setfill( '0' ) << dHours.count() << ':'
		<< std::fixed << std::setw( 2 ) << std::setfill( '0' ) << dMinutes.count() << ':'
		<< std::fixed << std::setw( 4 ) << std::setfill( '0' ) << std::setprecision( 1 ) << dSeconds.count();

	return os;
}

ProgressBar::Segment::Segment( std::string title, size_t weight, std::function<std::string( double )> extraDataGenerator ) :
	ProgressBar::SegmentBase( title, weight, extraDataGenerator ) {}

const std::string & ProgressBar::Segment::getTitle() const {
	return std::get<0>( *this );
}

const size_t & ProgressBar::Segment::getWeight() const {
	return std::get<1>( *this );
}

const ProgressBar::extraDataFunc & ProgressBar::Segment::getExtraDataGenerator() const {
	return std::get<2>( *this );
}

bool ProgressBar::Segment::hasExtraDataGenerator() const {
	return (bool)getExtraDataGenerator();
}
