#pragma once

#include <iomanip>
#include <sstream>
#include <string_view>
#include <vector>

#include "detail/split.h"

namespace gps_lib {
/**
 * @brief Checks if a given NMEA sentence is valid.
 * @param sample The NMEA sentence to validate.
 * @return  bool    True if the sample is valid, false otherwise.
 */
inline bool is_valid_sample(const std::string_view sample) {
  std::vector<std::string_view> tokens{detail::split(sample, '*')};

  if (tokens.size() < 2 || tokens.at(1).empty()) {
    return false;
  }

  std::string_view sentence = tokens.at(0);

  if (sentence.starts_with('$')) {
    sentence.remove_prefix(1);
  }

  unsigned char check = 0;

  for (char c : sentence) {
    check ^= static_cast<unsigned char>(c);
  }

  std::stringstream hex_check;

  hex_check << std::uppercase << std::hex << std::setfill('0') << std::setw(2)
            << static_cast<int>(check);

  return hex_check.str() == tokens.at(1);
}
} // namespace gps_lib
