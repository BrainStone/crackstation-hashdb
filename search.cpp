#include "search.h"

Match::Match( const HashLib::Hash & hash, const std::string & word, bool fullMatch ) :
	hash( hash ),
	word( word ),
	fullMatch( fullMatch ) {}

std::vector<Match> Match::getMatches( std::ifstream & wordlist, FileArray & index, HashLib * hashAlgorithm, const HashLib::Hash & hash ) {
	std::vector<Match> matches;
	HashLib::Hash comparisionHash;
	FileArray::IndexEntry searchElement;
	FileArray::IndexEntry centerElement;
	size_t lower = 0;
	size_t upper = index.getSize() - 1;
	size_t middle = 0;
	std::string line;

	const size_t hashSize( hash.getLength() );
	searchElement.setHash( hash );

	while ( lower < upper ) {
		middle = lower + (upper - lower) / 2;
		index.readEntry( centerElement, middle );

		if ( centerElement < searchElement )
			lower = middle + 1;
		else
			upper = middle;
	}

	while ( lower < index.getSize() ) {
		index.readEntry( centerElement, lower++ );

		if ( centerElement != searchElement )
			break;

		wordlist.seekg( centerElement.getOffset() );

		getline( wordlist, line );

		comparisionHash = hashAlgorithm->hash( line );

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

const HashLib::Hash & Match::getHash() const {
	return hash;
}

const std::string & Match::getWord() const {
	return word;
}

bool Match::isFullMatch() const {
	return fullMatch;
}

std::ostream & operator<<( std::ostream & rhs, const Match & lhs ) {
	rhs << lhs.toString();

	return rhs;
}
