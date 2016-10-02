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

// Typedefs & Structs
typedef std::lock_guard<std::mutex> scoped_lock;

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

std::string & removeChars( std::string & s, const std::string& chars );
std::string centerString( size_t width, const std::string& str );

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
