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
#include "progressbar.h"
#include "queue.h"
#include "util.h"

// Functions
void createIDX( const std::string & wordlist, const std::string & idxFile, const std::string & hash, size_t cores, bool quiet );
void readWordlist( Queue<std::pair<std::string, std::streampos>>* readQueue, std::ifstream* fileIn, size_t numHasherThreads );
void writeIndex( Queue<FileArray::IndexEntry>* writeQueue, std::ofstream* fileOut, size_t numHasherThreads );
void computeHashes( const std::string* hashName, Queue<std::pair<std::string, std::streampos>>* readQueue, Queue<FileArray::IndexEntry>* writeQueue, ProgressBar* progressBar, const std::streampos fileSize );

#endif
