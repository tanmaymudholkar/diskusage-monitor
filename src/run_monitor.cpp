#include "../include/run_monitor.h"

void run_tests() {
  std::cout << "Size of folder /home/tmudholk/codepractice is:" << std::endl
            << get_human_readable_representation(get_folder_size("/home/tmudholk/codepractice")) << std::endl;
}

int main(int argc, char* argv[]) {
  (void) argc;
  (void) argv;
  std::cout << "Hello, world!" << std::endl;
  run_tests();
  return EXIT_SUCCESS;
}
