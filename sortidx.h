#ifndef CRACKSTATION_SORTIDX_H
#define CRACKSTATION_SORTIDX_H

// Includes
#include <atomic>
#include <iostream>
#include <limits>
#include <string>
#include <thread>

#include "filearray.h"
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
void sortIDX( const std::string & idxFile, size_t cacheSize, bool quiet );

// Turns the idx file into a heap (first step of heapsort)
void heapifyIDX( size_t heapifyLimit );
// Sorts the idx heap (second step of heapsort)
void sortIDXHeap( size_t numDataSets );

// Checks whether a index is in the heap
bool isInHeap( size_t pos );
// Moves a element down the heap until it is at the right position
void orderHeap( IndexEntry &top, size_t posTop );

#endif
