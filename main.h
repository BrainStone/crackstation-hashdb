#ifndef CRACKSTATION_MAIN_H
#define CRACKSTATION_MAIN_H

// Includes
#include <iostream>
#include <string>
#include <unordered_map>
#include <vector>

#include "checkidx.h"
#include "createidx.h"
#include "optionparser.h"
#include "progressbar.h"
#include "search.h"
#include "sortidx.h"
#include "util.h"

// Constants
constexpr size_t defaultRam = 256;
constexpr int modeAll = 0b100;
constexpr int modeFull = 0b010;
constexpr int modePartial = 0b001;

constexpr int exitSucess = EXIT_SUCCESS;
constexpr int exitGenericFailure = EXIT_FAILURE;
constexpr int exitTestFailure = exitGenericFailure + 1;
constexpr int exitParseFailure = exitTestFailure + 1;

// Enums
enum optionIndex {
	UNKNOWN, HELP, CREATE, VERIFY, LIST, QUIET, RAM, TESTS_ALL, TESTS_FAST, TEST_SORTED, TEST_MATCH
};
enum programMode {
	MODE_USAGE, MODE_HELP, MODE_CREATE, MODE_CREATE_VERIFY, MODE_VERIFY, MODE_LIST, MODE_SEARCH
};
enum matchMode {
	MATCH_ALL = 0b111, MATCH_ALL_FULL = 0b110, MATCH_ALL_PARTIAL = 0b101, MATCH_RANDOM = 0b011, MATCH_RANDOM_FULL = 0b010, MATCH_RANDOM_PARTIAL = 0b001
};
extern const option::Descriptor usage[];

// Functions
int main( int argc, char** argv );
matchMode getMatchMode( std::string str );
option::ArgStatus isMatchMode( const option::Option& option, bool msg );

#endif
