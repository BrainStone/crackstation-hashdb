#include "createidx.h"
#include "checkidx.h"

bool runTestSorted( const std::string & idxFile, ProgressBar & progressBar, bool quiet ) {
	std::ifstream fileIn( idxFile, std::ios::in | std::ios::out | std::ios::binary | std::ios::ate );
	const size_t elements = fileIn.tellg() / FileArray::IndexEntry::indexSize;
	bool testPassed = true;

	std::unique_ptr<FileArray::IndexEntry> first( new FileArray::IndexEntry() );
	std::unique_ptr<FileArray::IndexEntry> last( new FileArray::IndexEntry() );

	fileIn >> *first;

	for ( size_t i = 0; i < elements; i++ ) {
		if ( !quiet )
			progressBar.updateProgress( 0, i, elements );

		fileIn >> *last;

		if ( *last < *first ) {
			testPassed = false;

			break;
		}

		std::swap( first, last );
	}

	if ( !quiet )
		progressBar.updateProgress( 0, elements, elements );

	return testPassed;
}
