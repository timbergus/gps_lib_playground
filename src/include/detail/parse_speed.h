#pragma once

#include <string>

#include "types.h"

namespace gps_lib::detail {
/**
 * @brief Parses a speed string in the format DDD.DD.
 * @param speed The speed string to parse.
 * @param units The units to convert to (ms or kmh).
 * @return  double  The parsed speed in knots.
 * @throws std::invalid_argument If the speed string is not a valid number.
 */
inline double parse_speed(const std::string speed, Units units) {
  double velocity = std::stod(std::string{speed});
  return units == Units::ms ? velocity * KNTOMS : velocity * KNTOKMH;
}
} // namespace gps_lib::detail
