#ifndef CRACKSTATION_CREATEIDX_H
#define CRACKSTATION_CREATEIDX_H

// Includes
#include <atomic>
#include <fstream>
#include <iostream>
#include <memory>
#include <mutex>
#include <stdexcept>
#include <string>
#include <vector>

#include <openssl/sha.h>

#include "filearray.h"
#include "util.h"

// Functions
void createIDX( const std::string & wordlist, const std::string & idxFile, const std::string & hash, size_t cores, bool quiet );
void computeHashes( std::atomic<bool>* threadReady, std::mutex* fileInMutex, std::mutex* fileOutMutex, std::ifstream* fileIn, std::ofstream* fileOut, std::unique_ptr<HashLib> hasher );

#endif
