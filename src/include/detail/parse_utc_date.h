#pragma once

#include <string>
#include <tuple>

namespace gps_lib::detail {
/**
 * @brief Parses a UTC date string in the format DDMMYY.
 * @param utc_date The UTC date string to parse.
 * @return  auto    A tuple containing the day, month, and year as strings.
 */
inline auto parse_utc_date(const std::string utc_date) {
  return std::tuple{
      std::string{utc_date.substr(0, 2)}, // Day.
      std::string{utc_date.substr(2, 2)}, // Month.
      std::string{utc_date.substr(4, 2)}, // Year.
  };
}
} // namespace gps_lib::detail
