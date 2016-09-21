#include "sortidx.h"

using namespace std;

streampos fileSize;
size_t numDataSets;
size_t limit;
atomic<size_t> pos;
fstream* file;
size_t arraySize = 0;
IndexEntry* cacheArray;

void readIntoArray( size_t numElements ) {
	if ( arraySize != 0 )
		writeFromArray();

	arraySize = numElements;
	cacheArray = new IndexEntry[arraySize];
	file->seekg( 0 );

	for ( size_t i = 0; i < arraySize; i++ ) {
		file->read( (char*)(cacheArray + i), writeSize );
	}
}

void writeFromArray() {
	file->seekp( 0 );

	for ( size_t i = 0; i < arraySize; i++ ) {
		file->write( (char*)(cacheArray + i), writeSize );
	}

	arraySize = 0;
	delete[] cacheArray;
}

void sortIDX( string idxFile, size_t cacheSize, bool quiet ) {
	file = new fstream( idxFile, ios::in | ios::out | ios::binary | ios::ate );
	fileSize = file->tellg();
	numDataSets = fileSize / writeSize;
	limit = numDataSets - 1;
	const size_t localLimit = limit;
	const size_t heapifyLimit = getParent( limit );
	thread* sorterThread;

	if ( !quiet )
		cout << "Sorting index (may take a while)...\n\33[sLoading cache from file..." << flush;

	cacheSize /= writeSize;
	readIntoArray( min(cacheSize, numDataSets) );

	sorterThread = new thread( heapifyIDX, heapifyLimit );

	if ( !quiet ) {
		cout << "\33[u";
		initProgress( heapifyLimit + localLimit, false );

		while ( pos <= heapifyLimit ) {
			this_thread::sleep_for( chrono::milliseconds( defaultTimeout ) );

			printProgress( (size_t)pos );
		}
	}

	sorterThread->join();
	delete sorterThread;

	pos = 0;
	sorterThread = new thread( sortIDXHeap, localLimit );

	if ( !quiet ) {
		while ( pos < localLimit ) {
			this_thread::sleep_for( chrono::milliseconds( defaultTimeout ) );

			printProgress( heapifyLimit + pos );
		}
	}

	sorterThread->join();
	delete sorterThread;

	if ( !quiet )
		cout << "\33[?25h\n\33[sSaving cache to file." << flush;

	writeFromArray();

	file->close();
	delete file;

	if ( !quiet )
		cout << "\33[u\33[KDone!" << endl;
}

void heapifyIDX( size_t heapifyLimit ) {
	IndexEntry top;
	size_t posTop;

	for ( pos = 0; pos <= heapifyLimit; pos++ ) {
		posTop = heapifyLimit - pos;

		readData( &top, posTop );

		orderHeap( top, posTop );
	}
}

void sortIDXHeap( size_t numDataSets ) {
	IndexEntry last;
	IndexEntry top;
	size_t posLast;
	size_t posTop;

	for ( pos = 0; pos < numDataSets; pos++ ) {
		posLast = numDataSets - pos;
		posTop = 0;
		limit = posLast - 1;

		readData( &last, posTop );
		readData( &top, posLast );
		writeData( &last, posLast );

		orderHeap( top, posTop );
	}
}

void readData( IndexEntry* entry, size_t pos ) {
	if ( pos < arraySize ) {
		*entry = cacheArray[pos];
	} else {
		file->seekg( pos * writeSize );
		file->read( (char*)entry, writeSize );
	}
}

void writeData( IndexEntry* entry, size_t pos ) {
	if ( pos < arraySize ) {
		cacheArray[pos] = *entry;
	} else {
		file->seekp( pos * writeSize );
		file->write( (char*)entry, writeSize );
	}
}

bool isInHeap( size_t pos ) {
	return pos <= limit;
}

void orderHeap( IndexEntry &top, size_t posTop ) {
	static IndexEntry left;
	static IndexEntry right;
	static size_t posLeft;
	static size_t posRight;
	static bool swapped;

	do {
		posLeft = getLeft( posTop );
		posRight = getRight( posTop );

		if ( isInHeap( posLeft ) ) {
			readData( &left, posLeft );

			if ( isInHeap( posRight ) ) {
				readData( &right, posRight );

				if ( right > left ) {
					if ( right > top ) {
						writeData( &right, posTop );
						posTop = posRight;

						swapped = true;
					} else {
						swapped = false;
					}
				} else {
					if ( left > top ) {
						writeData( &left, posTop );
						posTop = posLeft;

						swapped = true;
					} else {
						swapped = false;
					}
				}
			} else {
				if ( left > top ) {
					writeData( &left, posTop );
					posTop = posLeft;

					swapped = true;
				} else {
					swapped = false;
				}
			}
		} else {
			swapped = false;
		}
	} while ( swapped );

	writeData( &top, posTop );
}
