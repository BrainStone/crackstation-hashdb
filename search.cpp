#include "search.h"

Match::Match( const HashLib::Hash & hash, const std::string & word, bool fullMatch ) :
	hash( hash ),
	word( word ),
	fullMatch( fullMatch ) {}

std::vector<Match> Match::getMatches( const std::string & wordlist, const std::string & idxFile, const std::string & hashName, const std::string hashString ) {
	std::vector<Match> matches;
	FileArray fileArray( idxFile );
	const std::unique_ptr<HashLib> hasher( HashLib::getHasher( hashName ) );
	HashLib::Hash hash;
	HashLib::Hash comparisionHash;
	FileArray::IndexEntry searchElement;
	FileArray::IndexEntry centerElement;
	size_t lower = 0;
	size_t upper = fileArray.getSize() - 1;
	size_t middle = 0;
	std::ifstream fileIn( wordlist, std::ios::in );
	std::string line;

	hash.fromString( hashString );
	const size_t hashSize( hash.getLength() );
	searchElement.setHash( hash );

	while ( lower < upper ) {
		middle = lower + (upper - lower) / 2;
		fileArray.readEntry( centerElement, middle );

		if ( centerElement < searchElement )
			lower = middle + 1;
		else
			upper = middle;
	}

	while ( true ) {
		fileArray.readEntry( centerElement, lower++ );

		if ( centerElement != searchElement )
			break;

		fileIn.seekg( centerElement.getOffset() );
		getline( fileIn, line );

		comparisionHash = hasher->hash( line );

		if ( hashSize == comparisionHash.getLength() ) {
			if ( hash == comparisionHash )
				matches.push_back( Match( hash, line, true ) );
		} else {
			if ( hash.partialMatch( comparisionHash ) )
				matches.push_back( Match( comparisionHash, line, false ) );
		}
	}

	matches.shrink_to_fit();

	return matches;
}

std::string Match::toString() const {
	return hash.toString() + ": " + word + (fullMatch ? "" : " [partial]");
}

std::ostream & operator<<( std::ostream & rhs, const Match & lhs ) {
	rhs << lhs.toString();

	return rhs;
}
