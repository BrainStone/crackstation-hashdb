#include "createidx.h"

void createIDX( const std::string & wordlist, const std::string & idxFile, const std::string & hash, size_t cores, bool quiet ) {
	size_t i;
	const size_t numThreads = ((cores == 0) ? getNumCores() : cores) + 2;
	std::vector<std::thread> threads( numThreads );
	Queue<std::pair<std::string, std::streampos>> readQueue( MB );
	Queue<FileArray::IndexEntry> writeQueue( MB );
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
		if ( i == 0 )
			threads[i] = std::thread( readWordlist, &readQueue, &fileIn, numThreads - 2 );
		else if ( i == (numThreads - 1) )
			threads[i] = std::thread( writeIndex, &writeQueue, &fileOut, numThreads - 2 );
		else
			threads[i] = std::thread( computeHashes, &hash, &readQueue, &writeQueue, &progressBar, fileSize );
	}

	for ( i = 0; i < numThreads; i++ )
		threads[i].join();

	fileIn.close();
	fileOut.close();
}

void readWordlist( Queue<std::pair<std::string, std::streampos>>* readQueue, std::ifstream * fileIn, size_t numHasherThreads ) {
	std::string line;
	std::streampos pos;

	while ( !fileIn->eof() ) {
		pos = fileIn->tellg();
		getline( *fileIn, line );

		if ( !line.empty() )
			readQueue->push( std::make_pair( line, pos ) );
	}

	for ( size_t i = 0; i < numHasherThreads; i++ )
		readQueue->push( std::make_pair( "", -1 ) );

	std::cout << "readWordlist" << std::endl;
}

void writeIndex( Queue<FileArray::IndexEntry>* writeQueue, std::ofstream * fileOut, size_t numHasherThreads ) {
	FileArray::IndexEntry writeBuffer;
	FileArray::IndexEntry emptyHash;
	size_t numEmptyHashes = 0;

	emptyHash.setHash( HashLib::Hash() );

	while ( numEmptyHashes < numHasherThreads ) {
		writeQueue->pop( writeBuffer );

		if ( writeBuffer == emptyHash )
			numEmptyHashes++;
		else
			fileOut->write( reinterpret_cast<char*>(&writeBuffer), FileArray::IndexEntry::indexSize );
	}

	std::cout << "writeIndex" << std::endl;
}

void computeHashes( const std::string* hashName, Queue<std::pair<std::string, std::streampos>>* readQueue, Queue<FileArray::IndexEntry>* writeQueue, ProgressBar* progressBar, const std::streampos fileSize ) {
	std::pair<std::string, std::streampos> line;
	FileArray::IndexEntry writeBuffer;
	HashLib* hasher = HashLib::getHasher( *hashName );
	HashLib::Hash hash;

	while ( true ) {
		readQueue->pop( line );

		if ( line.second == -1 )
			hash = HashLib::Hash();
		else
			hash = hasher->hash( line.first );

		writeBuffer.setHash( hash );
		writeBuffer.setOffset( line.second );

		writeQueue->push( writeBuffer );

		if ( line.second == -1 )
			break;

		progressBar->updateProgress( 0, line.second, fileSize );
	}

	std::cout << "computeHashes" << std::endl;
}
