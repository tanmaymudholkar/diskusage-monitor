#ifndef H_FILESYSTEM_INTERFACE
#define H_FILESYSTEM_INTERFACE

#include <filesystem>
#include <cstdlib>
#include <iostream>
#include <cstdint>
#include <cassert>
#include <string>
#include <sstream>
#include <map>
#include <cmath>
#include <ctime>
#include <chrono>

#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>

std::string get_human_readable_representation(const unsigned long long int&);

unsigned long long int get_folder_size(const char*);

unsigned long long int get_size(const char*);

unsigned long long int get_last_modification_time(const char*);

enum class SizeUnit{B = 0, KB, MB, GB, TB};
const std::map<SizeUnit, std::string> SizeUnitNames = {
                                                 {SizeUnit::B, "B"},
                                                 {SizeUnit::KB, "KB"},
                                                 {SizeUnit::MB, "MB"},
                                                 {SizeUnit::GB, "GB"},
                                                 {SizeUnit::TB, "TB"}
};

#endif
