#ifndef CRACKSTATION_CREATEIDX_H
#define CRACKSTATION_CREATEIDX_H

// Includes
#include <atomic>
#include <fstream>
#include <iostream>
#include <mutex>
#include <string>
#include <vector>

#include <openssl/sha.h>

#include "filearray.h"
#include "util.h"

// Functions
void createIDX( const std::string & wordlist, const std::string & idxFile, const std::string & hash, long cores, bool quiet );
void computeHashes( std::atomic<bool>* threadReady, std::mutex* fileInMutex, std::mutex* fileOutMutex, std::ifstream* fileIn, std::ofstream* fileOut );

#endif
