#include "../include/run_monitor.h"

void run_tests() {
  std::cout << "Size of folder /home/tmudholk/codepractice is:" << std::endl
            << get_human_readable_representation(get_folder_size("/home/tmudholk/codepractice")) << std::endl;
  std::cout << "TwoPi: " << getTwoPi() << std::endl;
}

int main(int argc, char* argv[]) {
  assert (argc == 3);
  char* diskUsageCache = argv[1]; // Argument 1: path to file containing disk usage statistics
  char* testArgument = argv[2]; // Argument 2: test argument
  std::cout << "diskUsageCache: " << std::string(diskUsageCache) << ", testArgument: " << std::string(testArgument) << std::endl;
  run_tests();
  return EXIT_SUCCESS;
}
