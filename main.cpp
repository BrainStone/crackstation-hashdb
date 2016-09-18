#include "main.h"

using namespace std;

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

int main() {
	size_t i;
	bool runLoop = true;

	const size_t numThreads = getNumCores();

	thread threads[numThreads];
	atomic<bool> threadReady[numThreads];
	mutex fileInMutex;
	mutex fileOutMutex;
	ifstream fileIn( "test/words.txt", ios::in | ios::ate );
	ofstream fileOut( "test/words-sha512.idx", ios::out | ios::trunc );
	const streampos fileSize = fileIn.tellg();
	streampos pos;

	fileIn.seekg( 0 );

	for ( i = 0; i < numThreads; i++ ) {
		threadReady[i] = false;

		threads[i] = thread( computeHashes, &threadReady[i], &fileInMutex, &fileOutMutex, &fileIn, &fileOut );
	}

	while ( runLoop ) {
		{
			scoped_lock lock( fileInMutex );

			if ( fileIn.eof() )
				pos = fileSize;
			else
				pos = fileIn.tellg();

			cout << '\r' << pos << '/' << fileSize << '(' << ((double)pos / fileSize * 100.0) << "%)" << flush;
		}

		this_thread::sleep_for( chrono::milliseconds( 100 ) );

		for ( i = 0; i < numThreads; i++ )
			runLoop = runLoop && threadReady[i];

		runLoop = !runLoop;
	}

	cout << endl << "Joining threads!" << endl;

	for ( i = 0; i < numThreads; i++ )
		threads[i].join();

	fileIn.close();
	fileOut.close();

	cout << "End!" << endl;
}
