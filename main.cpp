#include "main.h"

using namespace std;

int main( int argc, char** argv ) {
	createIDX( "test/words.txt", "test/words-sha512.idx", "sha512" );
}
