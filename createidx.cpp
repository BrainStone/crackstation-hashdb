#include "createidx.h"

void createIDX( const std::string & wordlist, const std::string & idxFile, const std::string & hash, size_t cores, bool quiet ) {
	size_t i;
	const size_t numThreads = (cores == 0) ? getNumCores() : cores;
	std::vector<std::thread> threads( numThreads );
	std::vector<std::atomic<bool>> threadReady( numThreads );
	std::mutex fileInMutex;
	std::mutex fileOutMutex;
	std::ifstream fileIn( wordlist, std::ios::in | std::ios::ate );
	std::ofstream fileOut( idxFile, std::ios::out | std::ios::trunc );
	const std::streampos fileSize = fileIn.tellg();
	std::streampos pos;
	ProgressBar progressBar;

	if ( !fileIn.good() ) {
		throw std::invalid_argument( "File \"" + wordlist + "\" does not exist!" );
	}

	fileIn.seekg( 0 );

	if ( !quiet ) {
		std::cout << "Compiling wordlist " << wordlist << " into " << idxFile << " using the " << hash << " hash..." << std::endl;

		const unsigned short formatPower = getBytePower( fileSize );
		const std::string fileSizeString = getFormatedSize( fileSize, formatPower );

		progressBar.init( { {"Compiling wordlist...", 1} }, [fileSize, formatPower, fileSizeString]( double progress ) { return getFormatedSize( fileSize * progress, formatPower ) + " / " + fileSizeString; } );
		progressBar.start();
	}

	for ( i = 0; i < numThreads; i++ ) {
		threadReady[i] = false;

		threads[i] = std::thread( computeHashes, &threadReady[i], &fileInMutex, &fileOutMutex, &fileIn, &fileOut, fileSize, std::unique_ptr<HashLib>( HashLib::getHasher( hash ) ), &progressBar );
	}

	for ( i = 0; i < numThreads; i++ )
		threads[i].join();

	fileIn.close();
	fileOut.close();
}

void computeHashes( std::atomic<bool>* threadReady, std::mutex* fileInMutex, std::mutex* fileOutMutex, std::ifstream* fileIn, std::ofstream* fileOut, const std::streampos fileSize, std::unique_ptr<HashLib> hasher, ProgressBar* progressBar ) {
	std::string line;
	std::streampos pos;
	FileArray::IndexEntry writeBuffer;
	HashLib::Hash hash;

	while ( true ) {
		{
			scoped_lock lock( *fileInMutex );

			if ( fileIn->eof() )
				break;

			getline( *fileIn, line );
			pos = fileIn->tellg();
			progressBar->updateProgress( 0, pos, fileSize );
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
