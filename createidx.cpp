#include "createidx.h"

void createIDX( const std::string & wordlist, const std::string & idxFile, const std::string & hash, size_t cores, bool quiet ) {
	size_t i;
	bool runLoop = true;

	const size_t numThreads = (cores == 0) ? getNumCores() : cores;

	std::vector<std::thread> threads( numThreads );
	std::vector<std::atomic<bool>> threadReady( numThreads );
	std::mutex fileInMutex;
	std::mutex fileOutMutex;
	std::ifstream fileIn( wordlist, std::ios::in | std::ios::ate );
	std::ofstream fileOut( idxFile, std::ios::out | std::ios::trunc );
	const std::streampos fileSize = fileIn.tellg();
	std::streampos pos;

	if ( !fileIn.good() ) {
		throw std::invalid_argument( "File \"" + wordlist + "\" does not exist!" );
	}

	fileIn.seekg( 0 );

	if ( !quiet )
		std::cout << "Compiling wordlist " << wordlist << " into " << idxFile << " using the " << hash << " hash..." << std::endl;

	for ( i = 0; i < numThreads; i++ ) {
		threadReady[i] = false;

		threads[i] = std::thread( computeHashes, &threadReady[i], &fileInMutex, &fileOutMutex, &fileIn, &fileOut, std::unique_ptr<HashLib>( HashLib::getHasher( hash ) ) );
	}

	if ( !quiet ) {
		initProgress( fileSize, true );

		while ( runLoop ) {
			std::this_thread::sleep_for( std::chrono::milliseconds( defaultTimeout ) );

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

		std::cout << "\33[?25h" << std::endl;
	}

	for ( i = 0; i < numThreads; i++ )
		threads[i].join();

	fileIn.close();
	fileOut.close();

	if ( !quiet )
		std::cout << "Done!" << std::endl;
}

void computeHashes( std::atomic<bool>* threadReady, std::mutex* fileInMutex, std::mutex* fileOutMutex, std::ifstream* fileIn, std::ofstream* fileOut, std::unique_ptr<HashLib> hasher ) {
	std::string line;
	std::streampos pos;
	FileArray::IndexEntry writeBuffer;
	HashLib::Hash hash;

	while ( true ) {
		{
			scoped_lock lock( *fileInMutex );

			if ( fileIn->eof() )
				break;

			pos = fileIn->tellg();
			getline( *fileIn, line );
		}

		hash = hasher->hash( line );

		writeBuffer.setHash( hash );
		writeBuffer.setOffset( pos );

		{
			scoped_lock lock( *fileOutMutex );

			fileOut->write( (char*)&writeBuffer, FileArray::IndexEntry::indexSize );
		}
	}

	*threadReady = true;
}
