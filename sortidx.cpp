#include "sortidx.h"

using namespace std;

streampos fileSize;
size_t numDataSets;
size_t limit;
atomic<size_t> pos;

void sortIDX( string idxFile, bool quiet ) {
	if ( !quiet )
		cout << "Sorting index..." << endl;

	fstream file( idxFile, ios::in | ios::out | ios::binary | ios::ate );
	fileSize = file.tellg();
	numDataSets = fileSize / writeSize;
	limit = numDataSets - 1;
	const size_t heapifyLimit = getParent( limit );
	thread* sorterThread;

	sorterThread = new thread( sortStepOne, &file, heapifyLimit );

	if ( !quiet ) {
		initProgress( heapifyLimit, false );

		while ( pos < heapifyLimit ) {
			this_thread::sleep_for( chrono::milliseconds( 100 ) );

			printProgress( (size_t)pos );
		}

		printProgress( heapifyLimit );

		cout << "\33[?25h\nStep 1 complete." << endl;
	}

	sorterThread->join();
	delete sorterThread;


	file.close();

	if ( !quiet )
		cout << "Done!" << endl;
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

void sortStepTwo( std::fstream* file ) {

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
