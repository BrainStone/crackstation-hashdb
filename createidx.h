#ifndef CRACKSTATION_CREATEIDX_H
#define CRACKSTATION_CREATEIDX_H

// Includes
#include <fstream>
#include <iostream>
#include <stdexcept>
#include <string>

#include <openssl/sha.h>

#include "filearray.h"
#include "progressbar.h"
#include "util.h"

// Functions
void createIDX( const std::string & wordlist, const std::string & idxFile, const std::string & hashString, bool quiet );

#endif
