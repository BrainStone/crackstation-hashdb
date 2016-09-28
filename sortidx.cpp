#include "sortidx.h"

std::atomic<size_t> limit;

void sortIDX( const std::string & idxFile, size_t cacheByteSize, bool quiet ) {
	if ( !quiet )
		std::cout << "Sorting index (may take a while)..." << std::endl;

	{
		// Scoping for early destruction of objects
		ProgressBar progressBar;
		FileArray fileArray( idxFile, cacheByteSize / FileArray::IndexEntry::indexSize, &progressBar );
		const size_t localLimit = fileArray.getSize() - 1;
		limit = localLimit;
		const size_t heapifyLimit = getParent( localLimit );
		std::thread sorterThread;

		progressBar.init( {
			{ "Loading Cache", fileArray.getCacheSize() / 10 },
			{"Creating Heap", heapifyLimit / 3},
			{"Sorting Heap", fileArray.getSize() },
			{ "Savng Cache", fileArray.getCacheSize() / 10 }
		} );

		heapifyIDX( fileArray, progressBar, heapifyLimit );
		sortIDXHeap( fileArray, progressBar, localLimit );

		/*

		fileArray = &localFileArray;
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



		if ( !quiet ) {
			while ( pos < localLimit ) {
				std::this_thread::sleep_for( std::chrono::milliseconds( defaultTimeout ) );

				printProgress( heapifyLimit + pos );
			}
		}

		sorterThread.join();*/

		// Close the FileArray through RAII
	}

	if ( !quiet )
		std::cout << "Done!" << std::endl;
}

void heapifyIDX( FileArray & fileArray, ProgressBar & progressBar, size_t heapifyLimit ) {
	FileArray::IndexEntry top;
	size_t posTop;

	for ( size_t pos = 0; pos <= heapifyLimit; ) {
		posTop = heapifyLimit - pos;

		fileArray.readEntry( top, posTop );

		orderHeap( fileArray, top, posTop );

		progressBar.updateProgress( 1, ++pos, heapifyLimit + 1 );
	}
}

void sortIDXHeap( FileArray & fileArray, ProgressBar & progressBar, size_t numDataSets ) {
	FileArray::IndexEntry last;
	FileArray::IndexEntry top;
	size_t posLast;
	size_t posTop;

	for ( size_t pos = 0; pos < numDataSets; pos++ ) {
		posLast = numDataSets - pos;
		posTop = 0;
		limit = posLast - 1;

		fileArray.readEntry( last, posTop );
		fileArray.readEntry( top, posLast );
		fileArray.writeEntry( last, posLast );

		orderHeap( fileArray, top, posTop );

		progressBar.updateProgress( 2, ++pos, numDataSets );
	}
}

bool isInHeap( size_t pos ) {
	return pos <= limit;
}

void orderHeap( FileArray & fileArray, FileArray::IndexEntry &top, size_t posTop ) {
	static FileArray::IndexEntry left;
	static FileArray::IndexEntry right;
	static size_t posLeft;
	static size_t posRight;
	static bool swapped;

	do {
		posLeft = getLeft( posTop );
		posRight = getRight( posTop );

		if ( isInHeap( posLeft ) ) {
			fileArray.readEntry( left, posLeft );

			if ( isInHeap( posRight ) ) {
				fileArray.readEntry( right, posRight );

				if ( left < right ) {
					if ( top < right ) {
						fileArray.writeEntry( right, posTop );
						posTop = posRight;

						swapped = true;
					} else {
						swapped = false;
					}
				} else {
					if ( top < left ) {
						fileArray.writeEntry( left, posTop );
						posTop = posLeft;

						swapped = true;
					} else {
						swapped = false;
					}
				}
			} else {
				if ( top < left ) {
					fileArray.writeEntry( left, posTop );
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

	fileArray.writeEntry( top, posTop );
}
