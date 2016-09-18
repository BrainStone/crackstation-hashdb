#ifndef CRACKSTATION_MAIN_H
#define CRACKSTATION_MAIN_H

// Includes
#include <atomic>
#include <chrono>
#include <fstream>
#include <iostream>
#include <mutex>
#include <string>
#include <thread>

#include <openssl/sha.h>

#include "util.h"

// Constants
constexpr size_t hashSize = 8;
constexpr size_t offsetSize = 6;
constexpr size_t writeSize = hashSize + offsetSize;

// Typedefs
typedef std::lock_guard<std::mutex> scoped_lock;

// Functions
void computeHashes( std::atomic<bool>* threadReady, std::mutex* fileInMutex, std::mutex* fileOutMutex, std::ifstream* fileIn, std::ofstream* fileOut );
int main();

#endif
