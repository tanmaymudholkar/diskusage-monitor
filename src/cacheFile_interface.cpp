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

void cacheInfoClass::createGraphic(const char* outputGraphicFileName) {
  TCanvas outputCanvas("outputCanvas", "outputCanvas", 1024, 768);
  // TH1D sizesHistogram("sizes", ";backup object;size", objectsToMonitor_.size(), -0.5, -0.5 + objectsToMonitor_.size());
  // for (int objectIndex = 0; objectIndex < static_cast<int>(objectsToMonitor_.size()); ++objectIndex) {
  //   std::string objectToMonitor = objectsToMonitor_.at(objectIndex);
  //   sizesHistogram.GetXaxis()->SetBinLabel(1+objectIndex, objectToMonitor.c_str());
  //   sizesHistogram.SetBinContent(1+objectIndex, sizesUpdated_.at(objectToMonitor));
  //   sizesHistogram.SetBinError(1+objectIndex, 0.);
  // }
  // sizesHistogram.Draw();
  std::vector<int> colorsToUse{kBlue+2, kMagenta+2, kYellow+2, kGreen+2};
  unsigned long long int running_total_size = 0;
  for (const std::string& objectToMonitor: objectsToMonitor_) running_total_size += sizesUpdated_.at(objectToMonitor);
  // unsigned long long int totalBudget = 100*std::pow(static_cast<unsigned long long int>(1000), 3); // 100 GB budget
  unsigned long long int totalBudget = 10*std::pow(static_cast<unsigned long long int>(1000), 2); // 100 GB budget
  unsigned long long int freeSpace = 0;
  if (totalBudget > running_total_size) freeSpace = totalBudget - running_total_size;
  std::vector<double> diskUsageValues;
  std::vector<int> diskUsageColors;
  std::vector<std::string> diskUsageLabels;
  diskUsageValues.push_back(freeSpace);
  diskUsageColors.push_back(kGray+2);
  diskUsageLabels.push_back("Available: " + std::string(get_human_readable_representation(freeSpace)));
  double smallObjectsUsage = 0.;
  std::string smallObjectsLabel = "Others: ";
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
      smallObjectsLabel += objectToMonitor + ", ";
    }
  }
  smallObjectsLabel.pop_back();
  smallObjectsLabel.pop_back();
  diskUsageValues.push_back(smallObjectsUsage);
  diskUsageColors.push_back(colorsToUse.at(colorIndex%(colorsToUse.size())));
  diskUsageLabels.push_back(smallObjectsLabel);
  assert(diskUsageValues.size() == diskUsageColors.size());
  assert(diskUsageValues.size() == diskUsageLabels.size());

  std::vector<const char*> diskUsageLabelsFormatted;
  for (const std::string& diskUsageLabel: diskUsageLabels) diskUsageLabelsFormatted.push_back(diskUsageLabel.c_str());
  TPie pieChart("backupBudget", "Backup Budget", diskUsageValues.size(), &diskUsageValues[0], &diskUsageColors[0], &diskUsageLabelsFormatted[0]);
  pieChart.SetLabelFormat("#splitline{%txt}{(%perc)}");
  pieChart.SetLabelsOffset(-0.05);
  pieChart.Draw("T <");
  outputCanvas.SaveAs(outputGraphicFileName);
}
