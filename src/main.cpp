#include <cstdlib>
#include <print>

#include "json.h"
#include "parse.h"
#include "print.h"
#include "tools.h"

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
      gps_lib::print_sample(data);
    } else {
      std::println("Invalid sample: {}", line);
    }
  }

  std::string sample{
      "$GNRMC,211041.00,A,4024.98796,N,00340.22512,W,0.027,,010218,,,D*7B"};

  auto parsed_sample = gps_lib::parse(sample);

  if (parsed_sample) {
    gps_lib::save_to_json(*parsed_sample, "data/sample.json");
  } else {
    std::println("Failed to parse sample for JSON export.");
  }

  return EXIT_SUCCESS;
}
