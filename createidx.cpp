#include "createidx.h"

using namespace std;

streampos totalFileSize;
unsigned short formatPower;
string fileSizeString;

void computeHashes( atomic<bool>* threadReady, mutex* fileInMutex, mutex* fileOutMutex, ifstream* fileIn, ofstream* fileOut ) {
	size_t i;

	string line;
	streampos pos;
	unsigned char hash512[SHA512_DIGEST_LENGTH];
	unsigned char writeBuffer[writeSize];

	while ( !fileIn->eof() ) {
		{
			scoped_lock lock( *fileInMutex );

			pos = fileIn->tellg();
			getline( *fileIn, line );
		}

		SHA512( (const unsigned char*)line.c_str(), line.length(), hash512 );

		for ( i = 0; i < hashSize; i++ ) {
			writeBuffer[i] = hash512[i];
		}

		for ( i = 0; i < offsetSize; i++ ) {
			writeBuffer[i + hashSize] = getNthByte( pos, i );
		}

		{
			scoped_lock lock( *fileOutMutex );

			fileOut->write( (const char*)writeBuffer, writeSize );
		}
	}

	*threadReady = true;
}

void initProgress( streampos fileSize ) {
	totalFileSize = fileSize;
	formatPower = getBytePower( fileSize );
	fileSizeString = getFormatedSize( fileSize, formatPower );
}

void printProgress( streampos currentPos ) {
	int barWidth = getConsoleWidth() - 32;
	double progress = (double)currentPos / totalFileSize;

	cout << "\r\33[K[";
	int pos = barWidth * progress;
	for ( int i = 0; i < barWidth; ++i ) {
		if ( i < pos ) cout << "=";
		else if ( i == pos ) cout << ">";
		else cout << " ";
	}

	cout << "] " << int( progress * 100.0 ) << " %" << getFormatedSize( currentPos, formatPower ) << " of " << fileSizeString << flush;
}
