#include "../include/filesystem_interface.h"

std::string get_human_readable_representation(const unsigned long long int& size_bytes) {
  float size_float;
  SizeUnit unit;
  unsigned long long int OneThousand = static_cast<unsigned long long int>(1000);
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

unsigned long long int get_folder_size(const char* p) {
  std::filesystem::path folderPath = p;
  assert(std::filesystem::is_directory(folderPath));
  std::uintmax_t runningSize = 0;
  for (const auto& candidatePath: std::filesystem::recursive_directory_iterator(folderPath)) {
    if (std::filesystem::is_regular_file(candidatePath)) runningSize += std::filesystem::file_size(candidatePath);
  }
  return static_cast<unsigned long long int>(runningSize);
}

unsigned long long int get_size(const char* p) {
  std::filesystem::path fspath = p;
  std::filesystem::file_status fstatus = std::filesystem::status(p);
  assert(std::filesystem::status_known(fstatus));
  if (std::filesystem::is_directory(fspath)) return get_folder_size(p);
  else if (std::filesystem::is_symlink(fspath)) return 0;
  else if (std::filesystem::is_regular_file(fspath)) return static_cast<unsigned long long int>(std::filesystem::file_size(fspath));
  else {
    std::cerr << "ERROR: status of the following path is weird: " << std::string(p) << std::endl;
    std::exit(EXIT_FAILURE);
  }
}

unsigned long long int get_last_modification_time(const char* p) {
  std::filesystem::path fspath = p;
  std::filesystem::file_status fstatus = std::filesystem::status(p);
  assert(std::filesystem::status_known(fstatus));

  // I really tried for hours to get the latest modification time through std::filesystem::last_write_time()
  // but I couldn't find an easy way to convert "std::filesystem::file_time_type" to seconds since epoch.
  // Using "count" and the various chrono casts with duration gave negative values...

  // Here's the latest unsuccessful attempt: according to cppreference this may work in C++20
  // (if and when ROOT and gcc support it)
  // std::filesystem::file_time_type ftime = std::filesystem::last_write_time(p);
  // std::time_t cftime = std::chrono::system_clock::to_time_t(decltype(ftime)::clock::to_sys(ftime));
  // return static_cast<unsigned long long int>(cftime);

  // Using unix stat for now...
  struct stat objInfo;
  assert(stat(p, &objInfo) == 0);
  return static_cast<unsigned long long int>(*(&objInfo.st_mtime));
}
