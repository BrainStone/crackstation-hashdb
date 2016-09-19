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

void heapifyIDX( size_t heapifyLimit );
void sortIDXHeap( size_t numDataSets );

void readData( IndexEntry* entry, size_t pos );
void writeData( IndexEntry* entry, size_t pos );
bool isInHeap( size_t pos );
void orderHeap( IndexEntry &top, size_t posTop );

#endif
