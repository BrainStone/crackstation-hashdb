#ifndef CRACKSTATION_SEARCH_H
#define CRACKSTATION_SEARCH_H

#include <fstream>
#include <ostream>
#include <string>
#include <vector>

#include "filearray.h"
#include "hashlib.h"

class Match {
public:
	Match( const HashLib::Hash & hash, const std::string & word, bool fullMatch );

	static std::vector<Match> getMatches( const std::string & wordlist, const std::string & idxFile, const std::string & hashName, const std::string hashString );

	std::string toString() const;

	friend std::ostream & operator<<( std::ostream & rhs, const Match & lhs );

private:
	HashLib::Hash hash;
	std::string word;
	bool fullMatch;
};

#endif
