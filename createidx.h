#ifndef CRACKSTATION_CREATEIDX_H
#define CRACKSTATION_CREATEIDX_H

// Includes
#include <atomic>
#include <fstream>
#include <iostream>
#include <mutex>
#include <string>

#include <openssl/sha.h>

#include "util.h"

// Functions
void createIDX( std::string wordlist, std::string idxFile, std::string hash, bool quiet );
void computeHashes( std::atomic<bool>* threadReady, std::mutex* fileInMutex, std::mutex* fileOutMutex, std::ifstream* fileIn, std::ofstream* fileOut );

void initProgress( std::streampos fileSize );
void printProgress( std::streampos currentPos );

#endif
