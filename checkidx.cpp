#include "createidx.h"
#include "checkidx.h"

bool runTestSorted( FileArray & fileArray, ProgressBar & progressBar ) {
	const size_t elements = fileArray.getSize();
	bool testPassed = true;

	std::unique_ptr<FileArray::IndexEntry> first( new FileArray::IndexEntry() );
	std::unique_ptr<FileArray::IndexEntry> last( new FileArray::IndexEntry() );

	fileArray.readEntry( *first, 0 );

	for ( size_t i = 0; i < elements; i++ ) {
		progressBar.updateProgress( 0, i, elements );

		fileArray.readEntry( *last, 0 );

		if ( *last < *first ) {
			testPassed = false;

			break;
		}

		std::swap( first, last );
	}

	progressBar.updateProgress( 0, elements, elements );

	return testPassed;
}
