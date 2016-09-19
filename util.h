#ifndef CRACKSTATION_UTIL_H
#define CRACKSTATION_UTIL_H

// Includes
#include <iomanip>
#include <iostream>
#include <mutex>
#include <stddef.h>
#include <sstream>
#include <string>
#include <thread>
#include <unistd.h>

#include <sys/ioctl.h>

// Constants
constexpr size_t hashSize = 8;
constexpr size_t offsetSize = 6;
constexpr size_t writeSize = hashSize + offsetSize;
constexpr long long defaultTimeout = 100;

// Typedefs & Structs
typedef std::lock_guard<std::mutex> scoped_lock;

struct IndexEntry {
	unsigned char hash[hashSize]; // First 64 bits of the hash
	unsigned char position[offsetSize]; // Position of word in dictionary (48-bit little endian integer)
} __attribute__( (__packed__) );

// Functions
bool operator>( const IndexEntry &rhs, const IndexEntry &lhs );

/**
* Gets the nth byte from a var. The 0th byte is the right most byte numerically speaking.
* Or it gets the nth byte in little-endian.
*/
template<class T>
constexpr unsigned char getNthByte( T var, size_t pos ) {
	return (unsigned char)(var >> (pos * 8) & 0xFF);
}

struct winsize getConsoleSize();
unsigned short getConsoleHeight();
unsigned short getConsoleWidth();

size_t getNumCores();

unsigned short getBytePower( std::streampos size );
std::string getBytePowerPostfix( unsigned short power );
std::string getFormatedSize( std::streampos size, int power = -1 );

void initProgress( std::streampos fileSize, bool withFileSize );
void printProgress( std::streampos currentPos );

#endif
