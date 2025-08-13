#include <cstdlib>
#include <fstream>
#include <print>

#include "gps_lib.h"

int main() {
  std::ifstream file{"data/samples.txt"};

  if (!file) {
    std::println("Could not open file.");
    return EXIT_FAILURE;
  }

  std::string line;

  while (std::getline(file, line)) {
    if (gps_lib::is_valid_sample(line)) {
      auto data = gps_lib::parse(line);
      gps_lib::print_samples(data);
    } else {
      std::println("Invalid sample: {}", line);
    }
  }

  return EXIT_SUCCESS;
}
