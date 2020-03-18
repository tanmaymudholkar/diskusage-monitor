#ifndef CACHEFILE_INTERFACE
#define CACHEFILE_INTERFACE

#include <cstdlib>
#include <iostream>
#include <fstream>
#include <string>
#include <filesystem>
#include <map>
#include <vector>
#include <algorithm>
#include <iterator>

#include "TFile.h"
#include "TTree.h"
#include "TTreeReader.h"
#include "TTreeReaderValue.h"
#include "TCanvas.h"
#include "TAxis.h"
#include "TBox.h"
#include "TText.h"

#include "filesystem_interface.h"

class cacheInfoClass {
 public:
  cacheInfoClass(const char*);
  void readObjectInfoFromFile(const char*);
  void updateCache();
  void writeToFile(const char*);
  void saveImageToFile(const char*, const int&, const unsigned long long int&, const std::vector<unsigned long long int>&, const std::vector<int>&, const std::vector<std::string>&);
  void saveCacheAsGraphic(const char*);
  void printTest();
 private:
  std::vector<std::string> objectsToMonitor_;
  std::map<std::string, unsigned long long int> sizesFromCache_;
  std::map<std::string, unsigned long long int> lastModificationTimestampsFromCache_;
  std::map<std::string, unsigned long long int> sizesUpdated_;
  std::map<std::string, unsigned long long int> lastModificationTimestampsUpdated_;
  bool cacheFoundFromFile_;
};

#endif
