#include "../include/filesystem_interface.h"

std::string get_human_readable_representation(const std::uintmax_t& size_bytes) {
  float size_float;
  SizeUnit unit;
  std::uintmax_t OneThousand = static_cast<std::uintmax_t>(1000);
  assert(size_bytes < std::pow(OneThousand, 5));
  if (size_bytes > std::pow(OneThousand, 4)) {
    size_float = size_bytes/std::pow(OneThousand, 4);
    unit = SizeUnit::TB;
  }
  else if (size_bytes > std::pow(OneThousand, 3)) {
    size_float = size_bytes/std::pow(OneThousand, 3);
    unit = SizeUnit::GB;
  }
  else if (size_bytes > std::pow(OneThousand, 2)) {
    size_float = size_bytes/std::pow(OneThousand, 2);
    unit = SizeUnit::MB;
  }
  else if (size_bytes > OneThousand) {
    size_float = size_bytes/OneThousand;
    unit = SizeUnit::KB;
  }
  else {
    size_float = static_cast<float>(size_bytes);
    unit = SizeUnit::B;
  }
  std::stringstream readable_size;
  readable_size << std::fixed << std::setprecision(3) << size_float << " " << SizeUnitNames.at(unit);
  return readable_size.str();
}

std::uintmax_t get_folder_size(const char* p) {
  std::filesystem::path folderPath = p;
  assert(std::filesystem::is_directory(folderPath));
  std::uintmax_t runningSize = 0;
  for (const auto& candidatePath: std::filesystem::recursive_directory_iterator(folderPath)) {
    if (std::filesystem::is_regular_file(candidatePath)) runningSize += std::filesystem::file_size(candidatePath);
  }
  return runningSize;
}
