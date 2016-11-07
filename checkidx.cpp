#include "checkidx.h"

bool runTestSorted( FileArray & fileArray, ProgressBar & progressBar, bool quiet ) {
	const size_t elements = fileArray.getSize();
	bool testPassed = true;

	std::unique_ptr<FileArray::IndexEntry> first( new FileArray::IndexEntry() );
	std::unique_ptr<FileArray::IndexEntry> last( new FileArray::IndexEntry() );

	fileArray.readEntry( *first, 0 );

	for ( size_t i = 0; i < elements; i++ ) {
		if ( !quiet )
			progressBar.updateProgress( 0, i, elements );

		fileArray.readEntry( *last, i );

		if ( *last < *first ) {
			testPassed = false;

			break;
		}

		std::swap( first, last );
	}

	if ( !quiet )
		progressBar.updateProgress( 0, elements, elements );

	return testPassed;
}

bool runTestMatch( const std::string & wordlist, FileArray & fileArray, const std::string & hashAlgorithm, matchMode mode, ProgressBar & progressBar, bool quiet ) {
	std::ifstream fileWordlist( wordlist, std::ios::in | std::ios::ate );
	std::ifstream fileWordlist2( wordlist, std::ios::in | std::ios::ate );
	std::ifstream fileWordlist3( wordlist, std::ios::in | std::ios::ate );

	if ( !fileWordlist.good() )
		throw std::invalid_argument( "File \"" + wordlist + "\" does not exist!" );

	std::string line;
	HashLib::Hash hash;
	std::vector<Match> matches;
	const bool allMatch = mode & modeAll;
	const bool fullMatch = mode & modeFull;
	const bool partialMatch = mode & modePartial;
	const std::unique_ptr<HashLib> hasher( std::unique_ptr<HashLib>( HashLib::getHasher( hashAlgorithm ) ) );
	size_t i = 0;
	bool found;

	if ( allMatch ) {
		fileWordlist.seekg( 0 );

		while ( !fileWordlist.eof() ) {
			getline( fileWordlist, line );

			hash = hasher->hash( line );

			if ( fullMatch ) {
				found = false;

				for ( const Match & match : Match::getMatches( fileWordlist2, fileArray, hasher, hash ) ) {
					if ( match.isFullMatch() && (match.getWord() == line) ) {
						found = true;
						break;
					}
				}

				if ( !found )
					return false;
			}

			if ( partialMatch ) {
				found = false;

				for ( const Match & match : Match::getMatches( fileWordlist2, fileArray, hasher, hash.toString().substr( 0, FileArray::IndexEntry::hashSize * 2 ) ) ) {
					if ( !match.isFullMatch() && (match.getWord() == line) ) {
						found = true;
						break;
					}
				}

				if ( !found )
					return false;
			}

			if ( !quiet )
				progressBar.updateProgress( 1, ++i, fileArray.getSize() );
		}
	} else {
		std::random_device realRand;
		std::default_random_engine randEngine( realRand() );
		std::uniform_int_distribution<int> dist( 0, fileArray.getSize() - 1 );
	}

	return true;
}
