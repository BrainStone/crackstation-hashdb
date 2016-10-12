#ifndef CRACKSTATION_CHECKIDX_H
#define CRACKSTATION_CHECKIDX_H

#include <algorithm>
#include <fstream>
#include <memory>
#include <string>

#include "filearray.h"
#include "progressbar.h"

bool runTestSorted( const std::string & idxFile, ProgressBar & progressBar, bool quiet );

#endif
