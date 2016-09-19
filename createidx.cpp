#include "createidx.h"

using namespace std;

streampos totalFileSize;
unsigned short formatPower;
string fileSizeString;

void createIDX( string wordlist, string idxFile, string hash ) {
	cout << "Compiling wordlist " << wordlist << " into " << idxFile << " using the " << hash << " hash..." << endl;

	size_t i;
	bool runLoop = true;

	const size_t numThreads = getNumCores();

	thread threads[numThreads];
	atomic<bool> threadReady[numThreads];
	mutex fileInMutex;
	mutex fileOutMutex;
	ifstream fileIn( wordlist, ios::in | ios::ate );
	ofstream fileOut( idxFile, ios::out | ios::trunc );
	const streampos fileSize = fileIn.tellg();
	streampos pos;

	fileIn.seekg( 0 );

	for ( i = 0; i < numThreads; i++ ) {
		threadReady[i] = false;

		threads[i] = thread( computeHashes, &threadReady[i], &fileInMutex, &fileOutMutex, &fileIn, &fileOut );
	}

	initProgress( fileSize );

	while ( runLoop ) {
		this_thread::sleep_for( chrono::milliseconds( 100 ) );

		{
			scoped_lock lock( fileInMutex );

			if ( fileIn.eof() )
				pos = fileSize;
			else
				pos = fileIn.tellg();
		}

		printProgress( pos );

		for ( i = 0; i < numThreads; i++ )
			runLoop = runLoop && threadReady[i];

		runLoop = !runLoop;
	}

	cout << "\33[?25h" << endl;

	for ( i = 0; i < numThreads; i++ )
		threads[i].join();

	fileIn.close();
	fileOut.close();

	cout << "Done!" << endl;
}

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

	cout << "\33[?25l";
}

void printProgress( streampos currentPos ) {
	int barWidth = getConsoleWidth() - 35;
	double progress = (double)currentPos / totalFileSize;

	cout << "\33[s\33[K[";
	int pos = barWidth * progress;
	for ( int i = 0; i < barWidth; ++i ) {
		if ( i < pos ) cout << "=";
		else if ( i == pos ) cout << ">";
		else cout << " ";
	}

	cout << "] " << setw( 5 ) << fixed << setprecision( 1 ) << progress * 100.0 << "% "
		<< getFormatedSize( currentPos, formatPower ) << " / " << fileSizeString << "\33[u" << flush;
}