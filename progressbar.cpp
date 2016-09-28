#include "progressbar.h"

ProgressBar::ProgressBar() :
	initialized( false ),
	isRunning( false ),
	totalWeight( 0 ),
	thread( nullptr ),
	activeSegment( 0 ),
	segmentNames( 0 ),
	segmentWeights( 0 ),
	segmentProgresses( 0 ) {}

ProgressBar::ProgressBar( const std::vector<std::pair<std::string, size_t>>& segments, std::string( *extraDataGenerator )(double) ) : ProgressBar() {
	init( segments, extraDataGenerator );
}

ProgressBar::~ProgressBar() {
	finish( true );
}

void ProgressBar::init( const std::vector<std::pair<std::string, size_t>> & segments, std::string( *extraDataGenerator )(double) ) {
	if ( initialized )
		// Should I throw an exception?
		return;

	segmentNames.reserve( segments.size() );
	segmentWeights.reserve( segments.size() );
	segmentProgresses.resize( segments.size(), 0.0 );

	for ( const std::pair<std::string, size_t> & segment : segments ) {
		segmentNames.push_back( segment.first );
		segmentWeights.push_back( segment.second );

		totalWeight += segment.second;
	}

	this->extraDataGenerator = extraDataGenerator;

	initialized = true;
}

void ProgressBar::start() {
	std::cout << "\n\n\33[s\33[?25l";

	startTime = HRC::now();
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

	std::cout << "\33[?25h\n" << std::flush;
}

void ProgressBar::updateProgress( size_t numSegment, double progress ) {
	activeSegment = numSegment;

	scoped_lock lock( segmentProgressesMutex );

	segmentProgresses[numSegment] = progress;
}

void ProgressBar::updateProgress( size_t numSegment, size_t workDone, size_t workToDo ) {
	updateProgress( numSegment, div( workDone, workToDo ) );
}

int ProgressBar::getNumSegments() {
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
	duration timeElapsed = std::chrono::duration_cast<duration>(HRC::now() - startTime);
	duration timeRemaining = (1.0 / progress - 1.0) * timeElapsed;
	const size_t barWidth = getConsoleWidth();
	const size_t splitPos = barWidth * progress;
	const std::string percentString = getPercentString( progress, barWidth );

	std::cout << "\33[u\33[2A\33[K" << centerString( barWidth, "===== " + getActiveSegment() + "... =====" ) << '\n';
	std::cout << "\33[K\33[7m" << percentString.substr( 0, splitPos ) << "\33[0m" << percentString.substr( splitPos ) << '\n';
	std::cout << "\33[s\33[KTime elapsed: " << std::setw( 7 ) << std::fixed << std::setprecision( 1 ) << timeElapsed.count()
		<< "s\tTime remaining: " << std::setw( 7 ) << std::fixed << std::setprecision( 1 ) << timeRemaining.count() << "s";

	if ( extraDataGenerator != NULL )
		std::cout << "\t" << extraDataGenerator( progress );

	std::cout.flush();
}

double ProgressBar::getTotalProgress() {
	scoped_lock lock( segmentProgressesMutex );
	double progress = 0.0;

	for ( size_t i = 0; i < segmentProgresses.size(); i++ ) {
		progress += div( segmentWeights[i], totalWeight ) * segmentProgresses[i];
	}

	return progress;
}

const std::string & ProgressBar::getActiveSegment() {
	return segmentNames[activeSegment];
}
