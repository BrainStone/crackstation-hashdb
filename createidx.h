#ifndef CRACKSTATION_CREATEIDX_H
#define CRACKSTATION_CREATEIDX_H

// Includes
#include <atomic>
#include <fstream>
#include <mutex>
#include <string>

#include <openssl/sha.h>

#include "main.h"
#include "util.h"

// Functions
void computeHashes( std::atomic<bool>* threadReady, std::mutex* fileInMutex, std::mutex* fileOutMutex, std::ifstream* fileIn, std::ofstream* fileOut );

#endif