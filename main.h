#ifndef CRACKSTATION_MAIN_H
#define CRACKSTATION_MAIN_H

// Includes
#include <iostream>
#include <map>
#include <string>

#include "createidx.h"
#include "optionparser.h"
#include "progressbar.h"
#include "search.h"
#include "sortidx.h"
#include "util.h"

// Constants
constexpr size_t defaultRam = 256;

// Enums
enum optionIndex {
	UNKNOWN, HELP, CREATE, VERIFY, LIST, QUIET, RAM, TESTS_ALL, TESTS_FAST, TEST_SORTED, TEST_MATCH
};
enum programMode {
	MODE_HELP, MODE_CREATE, MODE_CREATE_VERIFY, MODE_VERIFY, MODE_LIST, MODE_SEARCH
};
enum matchMode {
	MATCH_ALL, MATCH_ALL_FULL, MATCH_ALL_PARTIAL, MATCH_RANDOM, MATCH_RANDOM_FULL, MATCH_RANDOM_PARTIAL
};
extern const option::Descriptor usage[];

// Functions
int main( int argc, char** argv );
matchMode getMatchMode( std::string str );
option::ArgStatus isMatchMode( const option::Option& option, bool msg );

#endif
