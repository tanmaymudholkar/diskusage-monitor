#include <filesystem>
#include <cstdint>
#include <cassert>
#include <string>
#include <sstream>
#include <map>
#include <cmath>

std::string get_human_readable_representation(const std::uintmax_t&);

std::uintmax_t get_folder_size(const char*);

enum class SizeUnit{B = 0, KB, MB, GB, TB};
const std::map<SizeUnit, std::string> SizeUnitNames = {
                                                 {SizeUnit::B, "B"},
                                                 {SizeUnit::KB, "KB"},
                                                 {SizeUnit::MB, "MB"},
                                                 {SizeUnit::GB, "GB"},
                                                 {SizeUnit::TB, "TB"}
};
