#include "sortidx.h"

using namespace std;

streampos fileSize;
size_t numDataSets;
size_t limit;
atomic<size_t> pos;

void sortIDX( string idxFile, bool quiet ) {
	if ( !quiet )
		cout << "Sorting index (may take a while)..." << endl;

	fstream file( idxFile, ios::in | ios::out | ios::binary | ios::ate );
	fileSize = file.tellg();
	numDataSets = fileSize / writeSize;
	limit = numDataSets - 1;
	const size_t localLimit = limit;
	const size_t heapifyLimit = getParent( limit );
	thread* sorterThread;

	sorterThread = new thread( sortStepOne, &file, heapifyLimit );

	if ( !quiet ) {
		initProgress( heapifyLimit + localLimit, false );

		while ( pos <= heapifyLimit ) {
			this_thread::sleep_for( chrono::milliseconds( 100 ) );

			printProgress( (size_t)pos );
		}

		printProgress( heapifyLimit );
	}

	sorterThread->join();
	delete sorterThread;

	pos = 0;
	sorterThread = new thread( sortStepTwo, &file, localLimit );

	if ( !quiet ) {
		while ( pos < localLimit ) {
			this_thread::sleep_for( chrono::milliseconds( 100 ) );

			printProgress( heapifyLimit + pos );
		}

		printProgress( heapifyLimit + localLimit );
	}

	sorterThread->join();
	delete sorterThread;

	file.close();

	if ( !quiet )
		cout << "\33[?25h\nSorting complete complete." << endl;
}

void sortStepOne( fstream* file, size_t heapifyLimit ) {
	IndexEntry top;
	size_t posTop;

	for ( pos = 0; pos <= heapifyLimit; pos++ ) {
		posTop = heapifyLimit - pos;

		readData( file, &top, posTop );

		orderHeap( file, top, posTop );
	}
}

void sortStepTwo( std::fstream* file, size_t numDataSets ) {
	IndexEntry last;
	IndexEntry top;
	size_t posLast;
	size_t posTop;

	for ( pos = 0; pos < numDataSets; pos++ ) {
		posLast = numDataSets - pos;
		posTop = 0;
		limit = posLast - 1;

		readData( file, &last, posTop );
		readData( file, &top, posLast );
		writeData( file, &last, posLast );

		orderHeap( file, top, posTop );
	}
}

void readData( fstream* file, IndexEntry* entry, size_t pos ) {
	file->seekg( pos * writeSize );
	file->read( (char*)entry, writeSize );
}

void writeData( fstream* file, IndexEntry* entry, size_t pos ) {
	file->seekp( pos * writeSize );
	file->write( (char*)entry, writeSize );
}

bool isInHeap( size_t pos ) {
	return pos <= limit;
}

void orderHeap( fstream* file, IndexEntry &top, size_t posTop ) {
	static IndexEntry left;
	static IndexEntry right;
	static size_t posLeft;
	static size_t posRight;
	static bool swapped;

	do {
		posLeft = getLeft( posTop );
		posRight = getRight( posTop );

		if ( isInHeap( posLeft ) ) {
			readData( file, &left, posLeft );

			if ( isInHeap( posRight ) ) {
				readData( file, &right, posRight );

				if ( right > left ) {
					if ( right > top ) {
						writeData( file, &right, posTop );
						posTop = posRight;

						swapped = true;
					} else {
						swapped = false;
					}
				} else {
					if ( left > top ) {
						writeData( file, &left, posTop );
						posTop = posLeft;

						swapped = true;
					} else {
						swapped = false;
					}
				}
			} else {
				if ( left > top ) {
					writeData( file, &left, posTop );
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

	writeData( file, &top, posTop );
}
