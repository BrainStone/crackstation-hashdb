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
	const size_t heapifyLimit = getParent( limit );
	thread* sorterThread;

	sorterThread = new thread( sortStepOne, &file, heapifyLimit );

	if ( !quiet ) {
		initProgress( heapifyLimit + limit, false );

		while ( pos < heapifyLimit ) {
			this_thread::sleep_for( chrono::milliseconds( 100 ) );

			printProgress( (size_t)pos );
		}

		printProgress( heapifyLimit );
	}

	sorterThread->join();
	delete sorterThread;

	pos = 0;
	sorterThread = new thread( sortStepTwo, &file, numDataSets );

	if ( !quiet ) {
		while ( pos < limit ) {
			this_thread::sleep_for( chrono::milliseconds( 100 ) );

			printProgress( heapifyLimit + pos );
		}

		printProgress( heapifyLimit + limit );
	}

	sorterThread->join();
	delete sorterThread;

	file.close();

	if ( !quiet )
		cout << "\33[?25h\nSorting complete complete." << endl;
}

void sortStepOne( fstream* file, size_t heapifyLimit ) {
	IndexEntry top;
	IndexEntry left;
	IndexEntry right;
	size_t posTop;
	size_t posLeft;
	size_t posRight;

	for ( pos = 0; pos < heapifyLimit; pos++ ) {
		posTop = heapifyLimit - pos;
		posLeft = getLeft( posTop );
		posRight = getRight( posTop );

		readData( file, &top, posTop );
		readData( file, &left, posLeft );

		if ( isInHeap( posRight ) ) {
			readData( file, &right, posRight );

			if ( right > left ) {
				if ( right > top ) {
					writeData( file, &right, posTop );
					writeData( file, &top, posRight );
				}
			} else {
				if ( left > top ) {
					writeData( file, &left, posTop );
					writeData( file, &top, posLeft );
				}
			}
		} else {
			if ( left > top ) {
				writeData( file, &left, posTop );
				writeData( file, &top, posLeft );
			}
		}
	}
}

void sortStepTwo( std::fstream* file, size_t numDataSets ) {
	IndexEntry* last = new IndexEntry();
	IndexEntry* top = new IndexEntry();
	IndexEntry* left = new IndexEntry();
	IndexEntry* right = new IndexEntry();
	size_t posLast;
	size_t posTop;
	size_t posLeft;
	size_t posRight;
	bool swaped;

	for ( pos = 0; pos < numDataSets; pos++ ) {
		posLast = numDataSets - pos;
		posTop = 0;

		readData( file, last, posTop );
		readData( file, top, posLast );
		writeData( file, last, posLast );

		do {
			posLeft = getLeft( posTop );
			posRight = getRight( posTop );

			if ( isInHeap( posLeft ) ) {
				readData( file, left, posLeft );

				if ( isInHeap( posRight ) ) {
					readData( file, right, posRight );

					if ( right > left ) {
						if ( right > top ) {
							writeData( file, right, posTop );
							posTop = posRight;

							swaped = true;
						} else {
							swaped = false;
						}
					} else {
						if ( left > top ) {
							writeData( file, left, posTop );
							posTop = posLeft;

							swaped = true;
						} else {
							swaped = false;
						}
					}
				} else {
					if ( left > top ) {
						writeData( file, left, posTop );
						posTop = posLeft;

						swaped = true;
					} else {
						swaped = false;
					}
				}
			} else {
				swaped = false;
			}
		} while ( swaped );

		writeData( file, top, posTop );
	}

	delete last;
	delete top;
	delete left;
	delete right;
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
