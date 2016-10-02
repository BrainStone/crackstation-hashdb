#include "createidx.h"

void createIDX( const std::string & wordlist, const std::string & idxFile, const std::string & hashString, bool quiet ) {
	std::ifstream fileIn( wordlist, std::ios::in | std::ios::ate );
	std::ofstream fileOut( idxFile, std::ios::out | std::ios::trunc );
	const std::streampos fileSize = fileIn.tellg();
	ProgressBar progressBar;
	std::unique_ptr<HashLib> hasher;
	std::streampos pos;
	std::string line;
	FileArray::IndexEntry writeBuffer;
	HashLib::Hash hash;

	if ( !fileIn.good() ) {
		throw std::invalid_argument( "File \"" + wordlist + "\" does not exist!" );
	}

	fileIn.seekg( 0 );

	if ( !quiet ) {
		std::cout << "Compiling wordlist " << wordlist << " into " << idxFile << " using the " << hashString << " hash..." << std::endl;

		const unsigned short formatPower = getBytePower( fileSize );
		const std::string fileSizeString = getFormatedSize( fileSize, formatPower );

		progressBar.init( { {"Compiling wordlist...", 1} }, [fileSize, formatPower, fileSizeString]( double progress ) { return getFormatedSize( fileSize * progress, formatPower ) + " / " + fileSizeString; } );
		progressBar.start();
	}

	hasher = std::unique_ptr<HashLib>( HashLib::getHasher( hashString ) );

	while ( !fileIn.eof() ) {
		pos = fileIn.tellg();
		getline( fileIn, line );

		hash = hasher->hash( line );

		writeBuffer.setHash( hash );
		writeBuffer.setOffset( pos );

		fileOut.write( (char*)&writeBuffer, FileArray::IndexEntry::indexSize );

		progressBar.updateProgress( 0, pos, fileSize );
	}

	fileIn.close();
	fileOut.close();
}
