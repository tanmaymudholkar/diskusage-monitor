#include "../include/cacheFile_interface.h"

template<typename KeyType, typename ValueType>
bool checkMapKeysAgainstVector(std::map<KeyType, ValueType> inputMap, std::vector<KeyType> inputVector) {
  if (inputMap.size() != inputVector.size()) return false;
  bool keysMatch = true;
  for (const KeyType& vectorElement: inputVector) {
    if (inputMap.find(vectorElement) == inputMap.end()) {
      keysMatch = false;
      break;
    }
  }
  return keysMatch;
}

cacheInfoClass::cacheInfoClass(const char* objectsToMonitorFile) {
  if (!(std::filesystem::is_regular_file(std::filesystem::path(objectsToMonitorFile)))) {
    std::cerr << "Unable to open file at path: " << objectsToMonitorFile << std::endl
              << "Recreating cache..." << std::endl;
    std::exit(EXIT_FAILURE);
  }
  std::ifstream objectsToMonitorFileStream(objectsToMonitorFile);
  assert(objectsToMonitorFileStream.is_open());
  std::string objectToMonitor;
  while (objectsToMonitorFileStream >> objectToMonitor) {
    objectsToMonitor_.push_back(objectToMonitor);
  }
  assert(!(objectsToMonitor_.empty()));
  cacheFoundFromFile_ = false;
}

void cacheInfoClass::readObjectInfoFromFile(const char* cacheFilePath) {
  if (!(std::filesystem::is_regular_file(std::filesystem::path(cacheFilePath)))) {
    std::cout << "Unable to locate file at path: " << cacheFilePath << std::endl
              << "Recreating cache..." << std::endl;
    return;
  }
  TFile* cacheFileContents = TFile::Open(cacheFilePath, "READ");
  assert((cacheFileContents->IsOpen()) && (!(cacheFileContents->IsZombie())));
  TTree* cacheTree = nullptr;;
  cacheFileContents->GetObject("cache", cacheTree);
  assert(cacheTree != nullptr);
  TTreeReader cacheTreeReader(cacheTree);
  TTreeReaderValue<std::string> path(cacheTreeReader, "path");
  TTreeReaderValue<unsigned long long int> size(cacheTreeReader, "size");
  TTreeReaderValue<unsigned long long int> lastModificationTimestamp(cacheTreeReader, "lastModificationTimestamp");
  while (cacheTreeReader.Next()) {
    if (std::find(std::begin(objectsToMonitor_), std::end(objectsToMonitor_), *path) != std::end(objectsToMonitor_)) {
      sizesFromCache_[*path] = *size;
      lastModificationTimestampsFromCache_[*path] = *lastModificationTimestamp;
    }
    else {
      std::cout << "Warning: ignoring this path which is in the ROOT input but not in the list of objects to monitor: " << *path << std::endl;
    }
  }
  cacheFileContents->Close();
  bool sizesFoundFromCache = checkMapKeysAgainstVector(sizesFromCache_, objectsToMonitor_);
  bool timestampsFoundFromCache = checkMapKeysAgainstVector(lastModificationTimestampsFromCache_, objectsToMonitor_);
  cacheFoundFromFile_ = (sizesFoundFromCache && timestampsFoundFromCache);
}

void cacheInfoClass::updateCache() {
  for (const std::string& objectToMonitor: objectsToMonitor_) {
    unsigned long long int latestLastModificationTimestamp = get_last_modification_time(objectToMonitor.c_str());
    lastModificationTimestampsUpdated_[objectToMonitor] = latestLastModificationTimestamp;

    if (cacheFoundFromFile_) {
      if (latestLastModificationTimestamp <= lastModificationTimestampsFromCache_.at(objectToMonitor)) {
        sizesUpdated_[objectToMonitor] = sizesFromCache_.at(objectToMonitor);
      }
      else {
        sizesUpdated_[objectToMonitor] = get_size(objectToMonitor.c_str());
      }
    }
    else {
      sizesUpdated_[objectToMonitor] = get_size(objectToMonitor.c_str());
    }
  }
  assert(checkMapKeysAgainstVector(lastModificationTimestampsUpdated_, objectsToMonitor_));
  assert(checkMapKeysAgainstVector(sizesUpdated_, objectsToMonitor_));
}

void cacheInfoClass::writeToFile(const char* cacheFilePath) {
  TFile* cacheFile = TFile::Open(cacheFilePath, "RECREATE");
  assert((cacheFile->IsOpen()) && (!(cacheFile->IsZombie())));

  TTree cacheTree("cache", "cache");
  std::string path;
  cacheTree.Branch("path", &path);
  unsigned long long int size;
  cacheTree.Branch("size", &size);
  unsigned long long int lastModificationTimestamp;
  cacheTree.Branch("lastModificationTimestamp", &lastModificationTimestamp);
  for (const std::string& objectToMonitor: objectsToMonitor_) {
    path = objectToMonitor;
    size = sizesUpdated_.at(path);
    lastModificationTimestamp = lastModificationTimestampsUpdated_.at(path);
    cacheTree.Fill();
  }
  cacheFile->WriteTObject(&cacheTree);
  cacheFile->Close();
}

void cacheInfoClass::saveImageToFile(const char* outputGraphicFileName, const int& nBoxesToDraw, const unsigned long long int& totalBudget, const std::vector<unsigned long long int>& diskUsageValues, const std::vector<int>& diskUsageColors, const std::vector<std::string>& diskUsageLabels) {
  TCanvas outputCanvas("outputCanvas", "outputCanvas", 4096, 400);
  TBox boxInterface;
  TText textInterface;
  textInterface.SetTextColor(kBlack);
  textInterface.SetTextAlign(22);
  textInterface.SetTextSize(0.1);
  textInterface.SetTextAngle(30);
  // first draw boxes
  double runningLeftEdge;
  runningLeftEdge = 0.;
  for (int index = 0; index < nBoxesToDraw; ++index) {
    double diskUsageFraction = (static_cast<double>(diskUsageValues.at(index))/static_cast<double>(totalBudget));
    boxInterface.SetLineColor(kBlack);
    boxInterface.SetFillColor(diskUsageColors.at(index));
    boxInterface.DrawBox(runningLeftEdge, 0., runningLeftEdge + diskUsageFraction, 1.);
    runningLeftEdge += diskUsageFraction;
  }
  // then draw labels
  runningLeftEdge = 0.;
  for (int index = 0; index < nBoxesToDraw; ++index) {
    double diskUsageFraction = (static_cast<double>(diskUsageValues.at(index))/static_cast<double>(totalBudget));
    textInterface.DrawTextNDC(runningLeftEdge + 0.5*diskUsageFraction, 0.5, diskUsageLabels.at(index).c_str());
    runningLeftEdge += diskUsageFraction;
  }
  outputCanvas.SaveAs(outputGraphicFileName);
}

void cacheInfoClass::saveCacheAsGraphic(const char* outputGraphicFileName) {
  std::vector<int> colorsToUse{kBlue+2, kMagenta+2, kYellow+2, kGreen+2};
  unsigned long long int running_total_size = 0;
  for (const std::string& objectToMonitor: objectsToMonitor_) running_total_size += sizesUpdated_.at(objectToMonitor);
  unsigned long long int totalBudget = 100*std::pow(static_cast<unsigned long long int>(1000), 3); // 100 GB budget
  // unsigned long long int totalBudget = 10*std::pow(static_cast<unsigned long long int>(1000), 2); // 10 MB budget
  unsigned long long int freeSpace = 0;
  if (totalBudget > running_total_size) freeSpace = totalBudget - running_total_size;
  std::vector<unsigned long long int> diskUsageValues;
  std::vector<int> diskUsageColors;
  std::vector<std::string> diskUsageLabels;
  unsigned long long int smallObjectsUsage = 0;
  unsigned int colorIndex = 0;
  for (const std::string& objectToMonitor: objectsToMonitor_) {
    unsigned long long int objectSize = sizesUpdated_.at(objectToMonitor);
    if (objectSize > 0.05*running_total_size) {
      diskUsageValues.push_back(objectSize);
      diskUsageColors.push_back(colorsToUse.at(colorIndex%(colorsToUse.size())));
      ++colorIndex;
      diskUsageLabels.push_back(objectToMonitor + ": " + std::string(get_human_readable_representation(objectSize)));
    }
    else {
      smallObjectsUsage += objectSize;
    }
  }
  diskUsageValues.push_back(smallObjectsUsage);
  diskUsageColors.push_back(colorsToUse.at(colorIndex%(colorsToUse.size())));
  diskUsageLabels.push_back("Others: " + std::string(get_human_readable_representation(smallObjectsUsage)));
  diskUsageValues.push_back(freeSpace);
  diskUsageColors.push_back(kGray+2);
  diskUsageLabels.push_back("Available: " + std::string(get_human_readable_representation(freeSpace)));
  assert(diskUsageValues.size() == diskUsageColors.size());
  assert(diskUsageValues.size() == diskUsageLabels.size());
  saveImageToFile(outputGraphicFileName, static_cast<int>(diskUsageLabels.size()), totalBudget, diskUsageValues, diskUsageColors, diskUsageLabels);
}

void cacheInfoClass::printTest() {
  for (const std::string& objectToMonitor: objectsToMonitor_) {
    std::cout << "At path: " << objectToMonitor << ", size: " << get_size(objectToMonitor.c_str()) << ", timestamp: " << get_last_modification_time(objectToMonitor.c_str()) << std::endl;
  }
}
