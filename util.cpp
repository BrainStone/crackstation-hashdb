#include "util.h"

std::streampos totalFileSize;
unsigned short formatPower;
std::string fileSizeString;
bool renderWithFileSize;

struct winsize getConsoleSize() {
	struct winsize size;
	ioctl( STDOUT_FILENO, TIOCGWINSZ, &size );

	return size;
}

unsigned short getConsoleHeight() {
	return getConsoleSize().ws_row;
}

unsigned short getConsoleWidth() {
	return getConsoleSize().ws_col;
}

size_t getNumCores() {
	size_t out = std::thread::hardware_concurrency();

	if ( out == 0 )
		return 1;
	else
		return out;
}

unsigned short getBytePower( std::streampos size ) {
	unsigned short power;

	for ( power = 0; size >= 1000; power++ )
		size = size >> 10;

	return power;
}

std::string getBytePowerPostfix( unsigned short power ) {
	static const std::string postfixes[] = { "  B", "KiB", "MiB", "GiB", "TiB", "PiB", "EiB", "ZiB", "YiB" };
	static constexpr size_t numPostfixes = sizeof( postfixes ) / sizeof( std::string );

	if ( power > numPostfixes ) {
		return std::string( "2^" ) + std::to_string( power * 10 ) + postfixes[0];
	} else {
		return postfixes[power];
	}
}

std::string getFormatedSize( std::streampos size, int power ) {
	unsigned short formatPower = (power <= -1) ? getBytePower( size ) : (unsigned short)power;

	std::stringstream ss;

	if ( power == 0 ) {
		ss << std::setw( 3 ) << size << "    ";
	} else {
		ss << std::setw( 7 ) << std::fixed << std::setprecision( 3 ) << double( size ) / double( 1 << (10 * power) );
	}

	ss << ' ' << getBytePowerPostfix( formatPower );

	return ss.str();
}

void initProgress( std::streampos fileSize, bool withFileSize ) {
	totalFileSize = fileSize;
	formatPower = getBytePower( fileSize );
	fileSizeString = getFormatedSize( fileSize, formatPower );
	renderWithFileSize = withFileSize;

	std::cout << "\33[?25l";
}

void printProgress( std::streampos currentPos ) {
	int barWidth = getConsoleWidth() - (renderWithFileSize ? 35 : 9);
	double progress = (double)currentPos / totalFileSize;

	std::cout << "\33[s\33[K[";
	int pos = barWidth * progress;
	for ( int i = 0; i < barWidth; ++i ) {
		if ( i < pos ) std::cout << '=';
		else if ( i == pos ) std::cout << '>';
		else std::cout << ' ';
	}

	std::cout << "] " << std::setw( 5 ) << std::fixed << std::setprecision( 1 ) << progress * 100.0 << '%';

	if ( renderWithFileSize )
		std::cout << ' ' << getFormatedSize( currentPos, formatPower ) << " / " << fileSizeString;

	std::cout << "\33[u" << std::flush;
}

option::ArgStatus Arg::Long( const option::Option& option, bool msg ) {
	try {
		std::stol( option.arg );

		return option::ARG_OK;
	} catch ( const std::exception & ) {
		if ( msg )
			std::cerr << "Option '" << option.name << "' requires a numeric argument\n" << std::endl;

		return option::ARG_ILLEGAL;
	}
}

option::ArgStatus Arg::ULong( const option::Option& option, bool msg ) {
	try {
		std::stoul( option.arg );

		return option::ARG_OK;
	} catch ( const std::exception & ) {
		if ( msg )
			std::cerr << "Option '" << option.name << "' requires a non-negative numeric argument\n" << std::endl;

		return option::ARG_ILLEGAL;
	}
}

std::string& removeChars( std::string& s, const std::string& chars ) {
	s.erase( std::remove_if( s.begin(), s.end(), [&chars]( const char& c ) {
		return chars.find( c ) != std::string::npos;
	} ), s.end() );

	return s;
}
