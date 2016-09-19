#ifndef CRACKSTATION_UTIL_H
#define CRACKSTATION_UTIL_H

// Includes
#include <iomanip>
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

// Typedefs
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

struct winsize getConsoleSize();
unsigned short getConsoleHeight();
unsigned short getConsoleWidth();

size_t getNumCores();

unsigned short getBytePower( std::streampos size );
std::string getBytePowerPostfix( unsigned short power );
std::string getFormatedSize( std::streampos size, int power = -1 );

#endif
