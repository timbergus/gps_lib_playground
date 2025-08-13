#pragma once

#include <vector>

namespace gps_lib::detail {
/**
 * @brief Splits a string_view into a vector of string_views based on a
 * separator.
 * @param sample The input string to split.
 * @param separator The character to split on.
 * @return  std::vector<std::string_view>  A vector containing the split tokens.
 * @note Returns an empty vector if the input is empty.
 */
inline std::vector<std::string_view> split(const std::string_view sample,
                                           char separator) {
  size_t start = 0;
  size_t end = 0;

  std::vector<std::string_view> tokens;

  while ((end = sample.find(separator, start)) != std::string::npos) {
    tokens.push_back(sample.substr(start, end - start));
    start = end + 1;
  }

  tokens.push_back(sample.substr(start));

  return tokens;
}
} // namespace gps_lib::detail
