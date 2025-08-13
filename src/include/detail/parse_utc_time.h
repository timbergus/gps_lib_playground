#pragma once

#include <string>
#include <tuple>

namespace gps_lib::detail {
/**
 * @brief Parses a UTC time string in the format HHMMSS.
 * @param utc_time The UTC time string to parse.
 * @return  auto    A tuple containing the hours, minutes, and seconds as
 * strings.
 */
inline auto parse_utc_time(const std::string utc_time) {
  return std::tuple{
      std::string{utc_time.substr(0, 2)}, // Hours.
      std::string{utc_time.substr(2, 2)}, // Minutes.
      std::string{utc_time.substr(4, 2)}, // Seconds.
  };
}
} // namespace gps_lib::detail
