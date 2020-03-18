#include "../include/run_monitor.h"

void run_tests(const argumentsStruct& inputArguments) {
  std::time_t currentTime = std::time(nullptr);
  std::cout << "objectsToMonitor: " << std::string(inputArguments.objectsToMonitor) << ", diskUsageCache: " << std::string(inputArguments.diskUsageCache) << std::endl;
  std::cout << "Size of folder /home/tmudholk/codepractice is:" << std::endl
            << get_human_readable_representation(get_folder_size("/home/tmudholk/codepractice")) << std::endl;
  std::cout << "Current time: " << std::asctime(std::localtime(&currentTime));
  std::cout << "Seconds since epoch: " << currentTime << std::endl;

  cacheInfoClass cacheInfo(inputArguments.objectsToMonitor);
  cacheInfo.readObjectInfoFromFile(inputArguments.diskUsageCache);
  cacheInfo.updateCache();
  cacheInfo.writeToFile(inputArguments.diskUsageCache);
}

int main(int argc, char* argv[]) {
  assert (argc == 4);
  argumentsStruct inputArguments(argv);
  // run_tests(inputArguments, currentTime);
  cacheInfoClass cacheInfo(inputArguments.objectsToMonitor);
  cacheInfo.readObjectInfoFromFile(inputArguments.diskUsageCache);
  // cacheInfo.printTest();
  cacheInfo.updateCache();
  cacheInfo.writeToFile(inputArguments.diskUsageCache);
  cacheInfo.saveCacheAsGraphic(inputArguments.outputGraphicFileName);
  return EXIT_SUCCESS;
}
