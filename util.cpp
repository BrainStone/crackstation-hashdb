#include "util.h"

using namespace std;

size_t getNumCores() {
	size_t out = thread::hardware_concurrency();

	if ( out == 0 )
		return 1;
	else
		return out;
}
