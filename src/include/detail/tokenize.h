#pragma once

#include <string_view>
#include <vector>

#include "split.h"

namespace gps_lib::detail {
/**
 * @brief Tokenizes a string_view into a vector of string_views based on the '*'
 * separator.
 * @param sample The input NMEA sentence to tokenize.
 * @return  std::vector<std::string_view>  A vector containing the tokenized
 * strings.
 */
inline std::vector<std::string_view> tokenize(const std::string_view sample) {
  std::vector<std::string_view> raw_data = detail::split(sample, '*');
  return detail::split(raw_data.at(0), ',');
}
} // namespace gps_lib::detail
