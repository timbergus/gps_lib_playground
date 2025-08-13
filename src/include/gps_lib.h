#pragma once

#include <concepts>
#include <cstddef>
#include <expected>
#include <iomanip>
#include <print>
#include <sstream>
#include <string>
#include <string_view>
#include <variant>
#include <vector>

namespace gps_lib {

/**
 * This are the speed units in meters per second and kilometers per hour.
 *
 * @var enum
 */
enum Units { ms, kmh };

/**
 * This enum represents the number of tokens for each NMEA sentence type.
 *
 * @var enum
 */
enum NumTokens {
  GGA_T = 15,
  GLL_T = 7,
  GSA_T = 18,
  GSV_T = 4,
  RMC_T = 12,
  VTG_T = 10,
  ZDA_T = 7
};

/**
 * This constant represents the conversion factor from knots to meters per
 * second.
 *
 * @var constexpr
 */
constexpr double KNTOMS{0.514444444};

/**
 * This constant represents the conversion factor from knots to kilometers per
 * hour.
 *
 * @var constexpr
 */
constexpr double KNTOKMH{1.85};

/**
 * This struct represents the latitude in a GPS coordinate.
 */
struct Latitude {
  double value;
  char direction; // 'N' or 'S'
};

/**
 * This struct represents the longitude in a GPS coordinate.
 */
struct Longitude {
  double value;
  char direction; // 'E' or 'W'
};

/**
 * This struct represents the GGA (Global Positioning System Fix Data) sentence.
 */
struct GGA {
  std::string type;
  std::string utc_time;
  Latitude latitude;
  Longitude longitude;
  std::string quality;
  std::string satellites_used;
  std::string hdop;
  std::string altitude;
  std::string geoidal_separation;
  std::string dgps;
};

/**
 * This struct represents the GLL (Geographic Latitude and Longitude) sentence.
 */
struct GLL {
  std::string type;
  Latitude latitude;
  Longitude longitude;
  std::string utc_time;
  std::string status;
};

/**
 * This struct represents the GSA (GNSS DOP and Active Satellites) sentence.
 */
struct GSA {
  std::string type;
  std::string mode;
  std::string fix_type;
  std::vector<std::string> satellites;
  std::string pdop;
  std::string hdop;
  std::string vdop;
  std::string checksum;
};

/**
 * This struct represents a satellite in the GPS system.
 */
struct Satellite {
  std::string id;
  std::string elevation;
  std::string azimuth;
  std::string snr;
};

/**
 * This struct represents the GSV (GNSS Satellites in View) sentence.
 */
struct GSV {
  std::string type;
  std::string number_of_messages;
  std::string sequence_number;
  std::string satellites_in_view;
  std::vector<Satellite> satellites;
};

/**
 * This struct represents the RMC (Recommended Minimum Specific GPS/Transit
 * Data) sentence.
 */
struct RMC {
  std::string type;
  std::string utc_time;
  std::string status;
  Latitude latitude;
  Longitude longitude;
  std::string speed;
  std::string course;
  std::string utc_date;
  std::string mode;
};

/**
 * This struct represents the VTG (Course Over Ground and Ground Speed)
 * sentence.
 */
struct VTG {
  std::string type;
  std::string course;
  std::string course_magnetic;
  std::string speed_kn;
  std::string speed_kh;
  std::string mode;
};

/**
 * This struct represents the ZDA (Time and Date) sentence.
 */
struct ZDA {
  std::string type;
  std::string utc_time;
  std::string utc_day;
  std::string utc_month;
  std::string utc_year;
  std::string local_zone_hours;
  std::string local_zone_minutes;
};

/**
 * Splits a string_view into a vector of string_views based on a separator.
 *
 * @return  std::vector<std::string_view>  A vector containing the split tokens.
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

/**
 * Tokenizes a string_view into a vector of string_views based on the '*'
 * separator.
 *
 * @return  std::vector<std::string_view>  A vector containing the tokenized
 * strings.
 */
inline std::vector<std::string_view> tokenize(const std::string_view sample) {
  std::vector<std::string_view> raw_data = split(sample, '*');
  return split(raw_data.at(0), ',');
}

/**
 * Checks if a given NMEA sentence is valid.
 *
 * @return  bool    True if the sample is valid, false otherwise.
 */
inline bool is_valid_sample(const std::string_view sample) {
  std::vector<std::string_view> tokens{split(sample, '*')};

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

/**
 * This enum represents the various parsing errors that can occur.
 *
 * @var enum
 */
enum ParseError {
  InvalidDirection,
  InvalidFormat,
  MissingFields,
  UnknownError,
  UnsupportedType,
};

/**
 * This variant represents a sample NMEA sentence.
 *
 * @var using
 */
using Sample = std::variant<GGA, GLL, GSA, GSV, RMC, VTG, ZDA>;

template <typename T>
concept StringLike = std::convertible_to<T, std::string_view>;
/**
 * Parses a given NMEA sentence and returns a Sample variant.
 *
 * @param sample  The NMEA sentence to parse.
 * @return std::expected<Sample, ParseError>  An expected containing the parsed
 * Sample or an error.
 */
inline std::expected<Sample, ParseError> parse(StringLike auto const &sample) {
  if (!is_valid_sample(sample)) {
    return std::unexpected(InvalidFormat);
  }

  auto tokens = tokenize(sample);

  if (tokens.empty()) {
    return std::unexpected(UnknownError);
  }

  // This won't throw an error because tokens is not empty.
  auto type = tokens.at(0);

  if (type.find("GGA") != std::string::npos) {
    if (tokens.size() < NumTokens::GGA_T) {
      return std::unexpected(MissingFields);
    }

    GGA data;

    data.type = tokens.at(0);
    data.utc_time = tokens.at(1);

    data.latitude.value = std::stod(std::string{tokens.at(2)});
    if (!tokens.at(3).empty() &&
        (tokens.at(3).front() == 'N' || tokens.at(3).front() == 'S')) {
      data.latitude.direction = tokens.at(3).front();
    } else {
      return std::unexpected{ParseError::InvalidDirection};
    }

    data.longitude.value = std::stod(std::string{tokens.at(4)});
    if (!tokens.at(5).empty() &&
        (tokens.at(5).front() == 'E' || tokens.at(5).front() == 'W')) {
      data.longitude.direction = tokens.at(5).front();
    } else {
      return std::unexpected{ParseError::InvalidDirection};
    }

    data.quality = tokens.at(6);
    data.satellites_used = tokens.at(7);
    data.hdop = tokens.at(8);
    data.altitude = tokens.at(9);
    data.geoidal_separation = tokens.at(11);
    data.dgps = tokens.at(14);

    return data;
  } else if (type.find("GLL") != std::string::npos) {
    if (tokens.size() < NumTokens::GLL_T) {
      return std::unexpected(MissingFields);
    }

    GLL data;

    data.type = tokens.at(0);

    data.latitude.value = std::stod(std::string{tokens.at(1)});
    if (!tokens.at(2).empty() &&
        (tokens.at(2).front() == 'N' || tokens.at(2).front() == 'S')) {
      data.latitude.direction = tokens.at(2).front();
    } else {
      return std::unexpected{ParseError::InvalidDirection};
    }

    data.longitude.value = std::stod(std::string{tokens.at(3)});
    if (!tokens.at(4).empty() &&
        (tokens.at(4).front() == 'E' || tokens.at(4).front() == 'W')) {
      data.longitude.direction = tokens.at(4).front();
    } else {
      return std::unexpected{ParseError::InvalidDirection};
    }

    data.utc_time = tokens.at(6);
    data.status = tokens.at(7);

    return data;
  } else if (type.find("GSA") != std::string::npos) {
    if (tokens.size() < NumTokens::GSA_T) {
      return std::unexpected(MissingFields);
    }

    GSA data;

    data.type = tokens.at(0);
    data.mode = tokens.at(1);
    data.fix_type = tokens.at(2);
    data.satellites.reserve(12);
    data.pdop = tokens.at(15);
    data.hdop = tokens.at(16);
    data.vdop = tokens.at(17);

    for (size_t i = 0; i < 12 && (i + 3) < tokens.size(); ++i) {
      data.satellites.push_back(std::string{tokens.at(i + 3)});
    }

    return data;
  } else if (type.find("GSV") != std::string::npos) {
    if (tokens.size() < NumTokens::GSV_T) {
      return std::unexpected(MissingFields);
    }

    GSV data;

    data.type = tokens.at(0);
    data.number_of_messages = tokens.at(1);
    data.sequence_number = tokens.at(2);
    data.satellites_in_view = tokens.at(3);

    data.satellites.reserve(std::stoi(data.number_of_messages));

    for (int i = 1; i <= std::stoi(data.number_of_messages) &&
                    static_cast<size_t>(i * 4 + 3) < tokens.size();
         ++i) {
      Satellite satellite;

      satellite.id = tokens[i * 4 + 0];
      satellite.elevation = tokens[i * 4 + 1];
      satellite.azimuth = tokens[i * 4 + 2];
      satellite.snr = tokens[i * 4 + 3];

      data.satellites.push_back(satellite);
    }

    return data;
  } else if (type.find("RMC") != std::string::npos) {
    if (tokens.size() < NumTokens::RMC_T) {
      return std::unexpected(MissingFields);
    }

    RMC data;

    data.type = tokens.at(0);
    data.utc_time = tokens.at(1);
    data.status = tokens.at(2);

    data.latitude.value = std::stod(std::string{tokens.at(3)});
    if (!tokens.at(4).empty() &&
        (tokens.at(4).front() == 'N' || tokens.at(4).front() == 'S')) {
      data.latitude.direction = tokens.at(4).front();
    } else {
      return std::unexpected{ParseError::InvalidDirection};
    }

    data.longitude.value = std::stod(std::string{tokens.at(5)});
    if (!tokens.at(6).empty() &&
        (tokens.at(6).front() == 'E' || tokens.at(6).front() == 'W')) {
      data.longitude.direction = tokens.at(6).front();
    } else {
      return std::unexpected{ParseError::InvalidDirection};
    }

    data.speed = tokens.at(7);
    data.course = tokens.at(8);
    data.utc_date = tokens.at(9);
    data.mode = tokens.at(11);

    return data;
  } else if (type.find("VTG") != std::string::npos) {
    if (tokens.size() < NumTokens::VTG_T) {
      return std::unexpected(MissingFields);
    }

    VTG data;

    data.type = tokens.at(0);
    data.course = tokens.at(1);
    data.course_magnetic = tokens.at(3);
    data.speed_kn = tokens.at(5);
    data.speed_kh = tokens.at(7);
    data.mode = tokens.at(9);

    return data;
  } else if (type.find("ZDA") != std::string::npos) {
    if (tokens.size() < NumTokens::ZDA_T) {
      return std::unexpected(MissingFields);
    }

    ZDA data;

    data.type = tokens.at(0);
    data.utc_time = tokens.at(1);
    data.utc_day = tokens.at(2);
    data.utc_month = tokens.at(3);
    data.utc_year = tokens.at(4);
    data.local_zone_hours = tokens.at(5);
    data.local_zone_minutes = tokens.at(6);

    return data;
  } else {
    return std::unexpected(UnsupportedType);
  }
}

/**
 * Parses a UTC date string in the format DDMMYY.
 *
 * @return  auto    A tuple containing the day, month, and year as strings.
 */
inline auto parse_utc_date(const std::string utc_date) {
  return std::tuple{
      std::string{utc_date.substr(0, 2)}, // Day.
      std::string{utc_date.substr(2, 2)}, // Month.
      std::string{utc_date.substr(4, 2)}, // Year.
  };
}

/**
 * Parses a UTC time string in the format HHMMSS.
 *
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

/**
 * Parses a latitude string in the format DDMM.MMMM.
 *
 * @return  double  The parsed latitude in decimal degrees.
 */
inline double parse_latitude(const std::string latitude) {
  return std::stod(std::string{latitude}) / 100.0;
}

/**
 * Parses a longitude string in the format DDDMM.MMMM.
 *
 * @return  double  The parsed longitude in decimal degrees.
 */
inline double parse_longitude(const std::string longitude,
                              const std::string longitude_direction) {
  double sign = longitude_direction == "W" ? -1.0 : 1.0;
  return sign * std::stod(std::string{longitude}) / 100.0;
}

/**
 * Parses a speed string in the format DDD.DD.
 *
 * @return  double  The parsed speed in knots.
 */
inline double parse_speed(const std::string speed, Units units) {
  double velocity = std::stod(std::string{speed});
  return units == ms ? velocity * KNTOMS : velocity * KNTOKMH;
}

/**
 * Prints the GGA data.
 *
 * @return  void    This function does not return a value.
 */
inline void print_gga(GGA data) {
  std::println("GGA: {}, {}, {}, {}, {}, {}, {}, {}, {}, {}", data.utc_time,
               data.latitude.value, data.latitude.direction,
               data.longitude.value, data.longitude.direction, data.quality,
               data.satellites_used, data.hdop, data.altitude,
               data.geoidal_separation);
}

/**
 * Prints the GSA data.
 *
 * @return  void    This function does not return a value.
 */
inline void print_gsa(GSA data) {
  std::println("GSA: {}, {}, {}, {}, {}, {}", data.mode, data.fix_type,
               data.satellites.size(), data.pdop, data.hdop, data.vdop);
  for (const auto &sat : data.satellites) {
    std::println("Satellite: {}", sat);
  }
}

/**
 * Prints the GSV data.
 *
 * @return  void    This function does not return a value.
 */
inline void print_gsv(GSV data) {
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
 * Prints the GLL data.
 *
 * @return  void    This function does not return a value.
 */
inline void print_gll(GLL data) {
  std::println("GLL: {}, {}, {}, {}, {}", data.latitude.value,
               data.latitude.direction, data.longitude.value,
               data.longitude.direction, data.utc_time, data.status);
}

/**
 * Prints the RMC data.
 *
 * @return  void    This function does not return a value.
 */
inline void print_rmc(RMC data) {
  std::println("RMC: {}, {}, {}, {}, {}, {}, {}, {}, {}, {}", data.utc_time,
               data.status, data.latitude.value, data.latitude.direction,
               data.longitude.value, data.longitude.direction, data.speed,
               data.course, data.utc_date, data.mode);
}

/**
 * Prints the VTG data.
 *
 * @return  void    This function does not return a value.
 */
inline void print_vtg(VTG data) {
  std::println("VTG: {}, {}, {}", data.course, data.speed_kn, data.speed_kh);
}

/**
 * Prints the ZDA data.
 *
 * @return  void    This function does not return a value.
 */
inline void print_zda(ZDA data) {
  std::println("ZDA: {}, {}, {}, {}, {}, {}", data.utc_time, data.utc_day,
               data.utc_month, data.utc_year, data.local_zone_hours,
               data.local_zone_minutes);
}

/**
 * Prints the parsed sample data.
 *
 * @return  void    This function does not return a value.
 */
inline void print_samples(const std::expected<Sample, ParseError> &sample) {
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
