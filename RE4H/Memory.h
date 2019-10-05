#ifndef __MEMORY__
#define __MEMORY__
#include <string>

using Pointer = char*;

Pointer patternScanHeap(const std::string &unformattedPattern);
Pointer patternScan(const std::string &unformattedPattern);

#endif