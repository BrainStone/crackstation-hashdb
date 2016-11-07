#ifndef CRACKSTATION_SEARCH_H
#define CRACKSTATION_SEARCH_H

#include <fstream>
#include <ostream>
#include <string>
#include <type_traits>
#include <vector>

#include "filearray.h"
#include "hashlib.h"

class Match {
public:
	Match( const HashLib::Hash & hash, const std::string & word, bool fullMatch );

	// Helper overloads
	template<class T1, class T2, class T3>
	inline static std::vector<Match> getMatches( const std::string & wordlist, T1 & index, const T2 & hashAlgorithm, const T3 & hash );
	template<class T1, class T2>
	inline static std::vector<Match> getMatches( std::ifstream & wordlist, const std::string & index, const T1 & hashAlgorithm, const T2 & hash );
	template<class T1>
	inline static std::vector<Match> getMatches( std::ifstream & wordlist, FileArray & index, const std::string & hashAlgorithm, const T1 & hash );
	template<class T1>
	inline static std::vector<Match> getMatches( std::ifstream & wordlist, FileArray & index, const std::unique_ptr<HashLib> & hashAlgorithm, const T1 & hash );
	inline static std::vector<Match> getMatches( std::ifstream & wordlist, FileArray & index, HashLib * hashAlgorithm, const std::string & hash );

	static std::vector<Match> getMatches( std::ifstream & wordlist, FileArray & index, HashLib * hashAlgorithm, const HashLib::Hash & hash );

	std::string toString() const;

	const HashLib::Hash & getHash() const;
	const std::string & getWord() const;
	bool isFullMatch() const;

	friend std::ostream & operator<<( std::ostream & rhs, const Match & lhs );

private:
	HashLib::Hash hash;
	std::string word;
	bool fullMatch;
};

template<class T1, class T2, class T3>
std::vector<Match> Match::getMatches( const std::string & wordlist, T1 & index, const T2 & hashAlgorithm, const T3 & hash ) {
	std::ifstream wordlistFile( wordlist, std::ios::in );

	if ( !wordlistFile.good() )
		throw std::invalid_argument( "File \"" + wordlist + "\" does not exist!" );

	return getMatches( wordlistFile, index, hashAlgorithm, hash );
}

template<class T1, class T2>
std::vector<Match> Match::getMatches( std::ifstream & wordlist, const std::string & index, const T1 & hashAlgorithm, const T2 & hash ) {
	FileArray fileArray( index );

	return getMatches( wordlist, fileArray, hashAlgorithm, hash );
}

template<class T1>
std::vector<Match> Match::getMatches( std::ifstream & wordlist, FileArray & index, const std::string & hashAlgorithm, const T1 & hash ) {
	const std::unique_ptr<HashLib> hasher( HashLib::getHasher( hashAlgorithm ) );

	return getMatches( wordlist, index, hasher, hash );
}

template<class T1>
std::vector<Match> Match::getMatches( std::ifstream & wordlist, FileArray & index, const std::unique_ptr<HashLib> & hashAlgorithm, const T1 & hash ) {
	return getMatches( wordlist, index, hashAlgorithm.get(), hash );
}

std::vector<Match> Match::getMatches( std::ifstream & wordlist, FileArray & index, HashLib * hashAlgorithm, const std::string & hash ) {
	HashLib::Hash realHash;
	realHash.fromString( hash );

	return getMatches( wordlist, index, hashAlgorithm, realHash );
}

#endif
