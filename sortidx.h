#ifndef CRACKSTATION_SORTIDX_H
#define CRACKSTATION_SORTIDX_H

// Includes
#include <algorithm>
#include <atomic>
#include <fstream>
#include <iostream>
#include <limits>
#include <string>
#include <thread>

#include "util.h"

// Functions
// Returns the parent index.
constexpr size_t getParent( size_t i ) {
	return (i - 1) / 2;
}

// Returns the left child index.
constexpr size_t getLeft( size_t i ) {
	return i * 2 + 1;
}

// Returns the right child index.
constexpr size_t getRight( size_t i ) {
	return i * 2 + 2;
}

// Sorts an idx file. Using chachSize bytes of RAM to speed it up.
void sortIDX( std::string idxFile, size_t cacheSize, bool quiet );

// Reads the specified number of elements into the cache
void readIntoCache( size_t numElements );
// Writes the cache to the file
void writeFromCache();

// Turns the idx file into a heap (first step of heapsort)
void heapifyIDX( size_t heapifyLimit );
// Sorts the idx heap (second step of heapsort)
void sortIDXHeap( size_t numDataSets );

// Reads data at the specified location. Either from cache or from disk.
void readData( IndexEntry* entry, size_t pos );
// Writes data at the specified location. Either to cache or to disk.
void writeData( IndexEntry* entry, size_t pos );
// Checks whether a index is in the heap
bool isInHeap( size_t pos );
// Moves a element down the heap until it is at the right position
void orderHeap( IndexEntry &top, size_t posTop );

#endif
