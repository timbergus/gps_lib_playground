#include <any>
#include <cstdlib>
#include <print>

#include "gps_lib.h"

int main() {
  std::string sample{"$GNRMC,211041.00,A,4024.98796,N,00340.22512,W,0.027,,"
                     "010218,,,D*7B"};

  auto result = GPS_LIB::is_valid_sample(sample);

  std::println("Is a valid sample? .. {}", result);

  auto data = std::any_cast<RMC>(GPS_LIB::parse(sample));

  std::println("Latitude: {}", GPS_LIB::parse_latitude(data.latitude));
  std::println("Longitude: {}", GPS_LIB::parse_longitude(
                                    data.longitude, data.longitude_direction));

  return EXIT_SUCCESS;
}
