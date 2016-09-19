#ifndef CRACKSTATION_SORTIDX_H
#define CRACKSTATION_SORTIDX_H

// Includes
#include <atomic>
#include <fstream>
#include <iostream>
#include <limits>
#include <string>
#include <thread>

#include "util.h"

// Functions
constexpr size_t getParent( size_t i ) {
	return (i - 1) / 2;
}

constexpr size_t getLeft( size_t i ) {
	return i * 2 + 1;
}

constexpr size_t getRight( size_t i ) {
	return i * 2 + 2;
}

void sortIDX( std::string idxFile, bool quiet );

void sortStepOne( std::fstream* file, size_t heapifyLimit );
void sortStepTwo( std::fstream* file, size_t numDataSets );

void readData( std::fstream* file, IndexEntry* entry, size_t pos );
void writeData( std::fstream* file, IndexEntry* entry, size_t pos );
bool isInHeap( size_t pos );
void orderHeap( std::fstream* file, IndexEntry &top, size_t posTop );

#endif
