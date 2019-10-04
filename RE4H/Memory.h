#ifndef __MEMORY__
#define __MEMORY__
#include <string>

typedef char* Pointer;

Pointer patternScanHeap(const std::string &unformattedPattern);
Pointer patternScan(const std::string &unformattedPattern);

#endif