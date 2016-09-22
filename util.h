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
constexpr long long defaultTimeout = 100;

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

struct winsize getConsoleSize();
unsigned short getConsoleHeight();
unsigned short getConsoleWidth();

size_t getNumCores();

// Determines which byte postfix to use (0 = "B", 1 = "KiB", ...)
unsigned short getBytePower( std::streampos size );
// Returns the appropriate byte postfix
std::string getBytePowerPostfix( unsigned short power );
// Formats the size. If power is -1 it automatically detects the power
std::string getFormatedSize( std::streampos size, int power = -1 );

// Initializes the progress bar
void initProgress( std::streampos fileSize, bool withFileSize );
// Prints the progressbar
void printProgress( std::streampos currentPos );

#endif
