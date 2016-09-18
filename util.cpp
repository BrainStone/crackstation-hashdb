#include "util.h"

using namespace std;

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
	size_t out = thread::hardware_concurrency();

	if ( out == 0 )
		return 1;
	else
		return out;
}

unsigned short getBytePower( streampos size ) {
	unsigned short power;

	for ( power = 0; size >= 1000; power++ )
		size = size >> 10;

	return power;
}

string getBytePowerPostfix( unsigned short power ) {
	static const string postfixes[] = { "  B", "KiB", "MiB", "GiB", "TiB", "PiB", "EiB", "ZiB", "YiB" };
	static constexpr size_t numPostfixes = sizeof( postfixes ) / sizeof( string );

	if ( power > numPostfixes ) {
		return string( "2^" ) + to_string( power * 10 ) + postfixes[0];
	} else {
		return postfixes[power];
	}
}

std::string getFormatedSize( std::streampos size, int power ) {
	unsigned short formatPower = (power <= -1) ? getBytePower( size ) : (unsigned short)power;

	stringstream ss;

	if ( power == 0 ) {
		ss << setw( 3 ) << size << "    ";
	} else {
		ss << setw( 7 ) << fixed << setprecision( 3 ) << double( size ) / double( 1 << (10 * power) );
	}

	ss << ' ' << getBytePowerPostfix( formatPower );

	return ss.str();
}
