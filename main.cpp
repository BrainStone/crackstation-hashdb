#include "main.h"

using namespace std;

int main() {
	size_t i;
	bool runLoop = true;

	const size_t numThreads = getNumCores();

	thread threads[numThreads];
	atomic<bool> threadReady[numThreads];
	mutex fileInMutex;
	mutex fileOutMutex;
	ifstream fileIn( /*"test/words.txt"*/"/home/platecheck/crackstation-hashdb/crackstation.txt", ios::in | ios::ate );
	ofstream fileOut( "test/words-sha512.idx", ios::out | ios::trunc );
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

	cout << endl << "Joining threads!" << endl;

	for ( i = 0; i < numThreads; i++ )
		threads[i].join();

	fileIn.close();
	fileOut.close();

	cout << "End!" << endl;
}
