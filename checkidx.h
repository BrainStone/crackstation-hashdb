#ifndef CRACKSTATION_CHECKIDX_H
#define CRACKSTATION_CHECKIDX_H

#include <algorithm>
#include <fstream>
#include <memory>
#include <random>
#include <string>

#include "createidx.h"
#include "filearray.h"
#include "progressbar.h"
#include "search.h"
#include "util.h"

bool runTestSorted( FileArray & fileArray, ProgressBar & progressBar, bool quiet );
bool runTestMatch( const std::string & wordlist, FileArray & fileArray, const std::string & hashAlgorithm, matchMode mode, ProgressBar & progressBar, bool quiet );

#endif
