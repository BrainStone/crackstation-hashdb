#ifndef CRACKSTATION_MAIN_H
#define CRACKSTATION_MAIN_H

// Includes
#include <iostream>
#include <string>

#include "createidx.h"
#include "optionparser.h"
#include "progressbar.h"
#include "search.h"
#include "sortidx.h"
#include "util.h"

// Constants
constexpr size_t defaultRam = 256;

// Functions
int main( int argc, char** argv );

#endif
