#include <cstdlib>
#include <string>
#include <iostream>
#include <cassert>
#include <ctime>

#include "filesystem_interface.h"
#include "cacheFile_interface.h"

struct argumentsStruct {
  char* objectsToMonitor; // Argument 1: path to file containing list of objects to monitor
  char* diskUsageCache; // Argument 2: path to file containing disk usage statistics
  char* outputGraphicFileName; // Argument 3: name of file in which to store output graphic

  argumentsStruct(char* argv[]) {
    objectsToMonitor = argv[1];
    diskUsageCache = argv[2];
    outputGraphicFileName = argv[3];
  }
};
