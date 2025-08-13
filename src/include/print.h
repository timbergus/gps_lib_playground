#pragma once

#include <expected>
#include <print>

#include "types.h"

/**
 * @namespace gps_lib
 * @brief A header-only C++ library for parsing and processing NMEA GPS
 * sentences.
 */
namespace gps_lib {
/**
 * @brief Prints the GGA data.
 * @param data The GGA data to print.
 * @return  void    This function does not return a value.
 */
inline void print_gga(const GGA &data) {
  std::println("GGA: {}, {}, {}, {}, {}, {}, {}, {}, {}, {}", data.utc_time,
               data.latitude.value, data.latitude.direction,
               data.longitude.value, data.longitude.direction, data.quality,
               data.satellites_used, data.hdop, data.altitude,
               data.geoidal_separation);
}

/**
 * @brief Prints the GSA data.
 * @param data The GSA data to print.
 * @return  void    This function does not return a value.
 */
inline void print_gsa(const GSA &data) {
  std::println("GSA: {}, {}, {}, {}, {}, {}", data.mode, data.fix_type,
               data.satellites.size(), data.pdop, data.hdop, data.vdop);
  for (const auto &sat : data.satellites) {
    std::println("Satellite: {}", sat);
  }
}

/**
 * @brief Prints the GSV data.
 * @param data The GSV data to print.
 * @return  void    This function does not return a value.
 */
inline void print_gsv(const GSV &data) {
  std::println("GSV: {}, {}, {}, {}", data.number_of_messages,
               data.sequence_number, data.satellites_in_view,
               data.satellites.size());
  for (const auto &satellite : data.satellites) {
    std::println("Satellite ID: {}, Elevation: {}, Azimuth: {}, SNR: {}",
                 satellite.id, satellite.elevation, satellite.azimuth,
                 satellite.snr);
  }
}

/**
 * @brief Prints the GLL data.
 * @param data The GLL data to print.
 * @return  void    This function does not return a value.
 */
inline void print_gll(const GLL &data) {
  std::println("GLL: {}, {}, {}, {}, {}", data.latitude.value,
               data.latitude.direction, data.longitude.value,
               data.longitude.direction, data.utc_time, data.status);
}

/**
 * @brief Prints the RMC data.
 * @param data The RMC data to print.
 * @return  void    This function does not return a value.
 */
inline void print_rmc(const RMC &data) {
  std::println("RMC: {}, {}, {}, {}, {}, {}, {}, {}, {}, {}", data.utc_time,
               data.status, data.latitude.value, data.latitude.direction,
               data.longitude.value, data.longitude.direction, data.speed,
               data.course, data.utc_date, data.mode);
}

/**
 * @brief Prints the VTG data.
 * @param data The VTG data to print.
 * @return  void    This function does not return a value.
 */
inline void print_vtg(const VTG &data) {
  std::println("VTG: {}, {}, {}", data.course, data.speed_kn, data.speed_kh);
}

/**
 * @brief Prints the ZDA data.
 * @param data The ZDA data to print.
 * @return  void    This function does not return a value.
 */
inline void print_zda(const ZDA &data) {
  std::println("ZDA: {}, {}, {}, {}, {}, {}", data.utc_time, data.utc_day,
               data.utc_month, data.utc_year, data.local_zone_hours,
               data.local_zone_minutes);
}

/**
 * @brief Prints the parsed sample data.
 * @param sample The parsed sample to print.
 * @return  void    This function does not return a value.
 */
inline void print_sample(const std::expected<Sample, ParseError> &sample) {
  if (!sample) {
    std::println("Error parsing sample to print.");
    return;
  } else {
    std::visit(
        [](auto &&data) {
          using T = std::decay_t<decltype(data)>;
          if constexpr (std::is_same_v<T, GSA>) {
            print_gsa(data);
          } else if constexpr (std::is_same_v<T, RMC>) {
            print_rmc(data);
          } else if constexpr (std::is_same_v<T, GGA>) {
            print_gga(data);
          } else if constexpr (std::is_same_v<T, GLL>) {
            print_gll(data);
          } else if constexpr (std::is_same_v<T, GSV>) {
            print_gsv(data);
          } else if constexpr (std::is_same_v<T, VTG>) {
            print_vtg(data);
          } else if constexpr (std::is_same_v<T, ZDA>) {
            print_zda(data);
          } else {
            std::println("Unknown or unsupported sample type.");
          }
        },
        sample.value());
  }
}
} // namespace gps_lib
