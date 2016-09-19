#include "util.h"

using namespace std;

streampos totalFileSize;
unsigned short formatPower;
string fileSizeString;
bool renderWithFileSize;

bool operator>( const IndexEntry &rhs, const IndexEntry &lhs ) {
	for ( size_t i = 0; i < hashSize; i++ ) {
		if ( rhs.hash[i] > lhs.hash[i] )
			return true;
		else if ( rhs.hash[i] < lhs.hash[i] )
			return false;
	}

	return false;
}

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

void initProgress( streampos fileSize, bool withFileSize ) {
	totalFileSize = fileSize;
	formatPower = getBytePower( fileSize );
	fileSizeString = getFormatedSize( fileSize, formatPower );
	renderWithFileSize = withFileSize;

	cout << "\33[?25l";
}

void printProgress( streampos currentPos ) {
	int barWidth = getConsoleWidth() - (renderWithFileSize ? 35 : 9);
	double progress = (double)currentPos / totalFileSize;

	cout << "\33[s\33[K[";
	int pos = barWidth * progress;
	for ( int i = 0; i < barWidth; ++i ) {
		if ( i < pos ) cout << '=';
		else if ( i == pos ) cout << '>';
		else cout << ' ';
	}

	cout << "] " << setw( 5 ) << fixed << setprecision( 1 ) << progress * 100.0 << '%';

	if ( renderWithFileSize )
		cout << ' ' << getFormatedSize( currentPos, formatPower ) << " / " << fileSizeString;

	cout << "\33[u" << flush;
}
