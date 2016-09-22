#include "sortidx.h"

std::atomic<size_t> limit;
std::atomic<size_t> pos;
FileArray* fileArray;

void sortIDX( const std::string & idxFile, size_t cacheSize, bool quiet ) {
	if ( !quiet )
		std::cout << "Sorting index (may take a while)...\n\33[sLoading cache from file..." << std::flush;

	{
		// Scoping for early destruction of objects
		FileArray fileArray( idxFile, cacheSize );
		const size_t localLimit = fileArray.getSize() - 1;
		limit = localLimit;
		const size_t heapifyLimit = getParent( localLimit );
		std::thread sorterThread;

		sorterThread = std::thread( heapifyIDX, heapifyLimit );

		if ( !quiet ) {
			std::cout << "\33[u";
			initProgress( heapifyLimit + localLimit, false );

			while ( pos <= heapifyLimit ) {
				std::this_thread::sleep_for( std::chrono::milliseconds( defaultTimeout ) );

				printProgress( (size_t)pos );
			}
		}

		sorterThread.join();

		pos = 0;
		sorterThread = std::thread( sortIDXHeap, localLimit );

		if ( !quiet ) {
			while ( pos < localLimit ) {
				std::this_thread::sleep_for( std::chrono::milliseconds( defaultTimeout ) );

				printProgress( heapifyLimit + pos );
			}
		}

		sorterThread.join();

		if ( !quiet )
			std::cout << "\33[?25h\n\33[sSaving cache to file." << std::flush;

		// Close the FileArray through RAII
	}

	if ( !quiet )
		std::cout << "\33[u\33[KDone!" << std::endl;
}

void heapifyIDX( size_t heapifyLimit ) {
	IndexEntry top;
	size_t posTop;

	for ( pos = 0; pos <= heapifyLimit; pos++ ) {
		posTop = heapifyLimit - pos;

		fileArray->readEntry( top, posTop );

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

		fileArray->readEntry( last, posTop );
		fileArray->readEntry( top, posLast );
		fileArray->writeEntry( last, posLast );

		orderHeap( top, posTop );
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
			fileArray->readEntry( left, posLeft );

			if ( isInHeap( posRight ) ) {
				fileArray->readEntry( right, posRight );

				if ( left < right ) {
					if ( top < right ) {
						fileArray->writeEntry( right, posTop );
						posTop = posRight;

						swapped = true;
					} else {
						swapped = false;
					}
				} else {
					if ( top < left ) {
						fileArray->writeEntry( left, posTop );
						posTop = posLeft;

						swapped = true;
					} else {
						swapped = false;
					}
				}
			} else {
				if ( top < left ) {
					fileArray->writeEntry( left, posTop );
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

	fileArray->writeEntry( top, posTop );
}
