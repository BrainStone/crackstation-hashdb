#ifndef CRACKSTATION_MAIN_H
#define CRACKSTATION_MAIN_H

// Includes
#include <iostream>
#include <math.h>
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

constexpr int exitSucess = EXIT_SUCCESS;
constexpr int exitGenericFailure = EXIT_FAILURE;
constexpr int exitTestFailure = exitGenericFailure + 1;
constexpr int exitParseFailure = exitTestFailure + 1;

// Variables
extern const option::Descriptor usage[];

// Functions
int main( int argc, char** argv );
matchMode getMatchMode( std::string str );
option::ArgStatus isMatchMode( const option::Option& option, bool msg );

#endif
