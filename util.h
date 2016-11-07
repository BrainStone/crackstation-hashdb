#ifndef CRACKSTATION_UTIL_H
#define CRACKSTATION_UTIL_H

// Includes
#include <algorithm>
#include <exception>
#include <iomanip>
#include <iostream>
#include <mutex>
#include <sstream>
#include <string>

#include "optionparser.h"

// Constants and Constant Expressions
constexpr size_t MB = 1024 * 1024;

constexpr int modeAll = 0b100;
constexpr int modeFull = 0b010;
constexpr int modePartial = 0b001;

// Typedefs & Structs
typedef unsigned char byte;

// Enums
enum optionIndex {
	UNKNOWN, HELP, CREATE, VERIFY, LIST, QUIET, RAM, TESTS_ALL, TESTS_FAST, TEST_SORTED, TEST_MATCH
};
enum programMode {
	MODE_USAGE, MODE_HELP, MODE_CREATE, MODE_CREATE_VERIFY, MODE_VERIFY, MODE_LIST, MODE_SEARCH
};
enum matchMode {
	MATCH_ALL = 0b111, MATCH_ALL_FULL = 0b110, MATCH_ALL_PARTIAL = 0b101, MATCH_RANDOM = 0b011, MATCH_RANDOM_FULL = 0b010, MATCH_RANDOM_PARTIAL = 0b001
};

// Functions
/**
* Gets the nth byte from a var. The 0th byte is the right most byte numerically speaking.
* Or it gets the nth byte in little-endian.
*/
template<class T>
constexpr unsigned char getNthByte( T var, size_t pos ) {
	return (unsigned char)(var >> (pos * 8) & 0xFF);
}

// Determines which byte postfix to use (0 = "B", 1 = "KiB", ...)
unsigned short getBytePower( std::streampos size );
// Returns the appropriate byte postfix
std::string getBytePowerPostfix( unsigned short power );
// Formats the size. If power is -1 it automatically detects the power
std::string getFormatedSize( std::streampos size, int power = -1 );

struct Arg : public option::Arg {
	static option::ArgStatus Long( const option::Option& option, bool msg );
	static option::ArgStatus ULong( const option::Option& option, bool msg );
};

std::string & strToLower( std::string & s );
std::string & strToUpper( std::string & s );
std::string & removeChars( std::string & s, const std::string& chars );
std::string & keepChars( std::string & s, const std::string& chars );
byte hexCharToByte( char c );

template <typename T>
std::string join( const T & v, const std::string & delim ) {
	std::ostringstream s;

	for ( const auto & i : v ) {
		if ( &i != &v[0] )
			s << delim;

		s << i;
	}

	return s.str();
}

#endif
