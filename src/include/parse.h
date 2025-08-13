#pragma once

#include <concepts>
#include <cstddef>
#include <expected>
#include <string>
#include <string_view>

#include "detail/tokenize.h"
#include "tools.h"
#include "types.h"

/**
 * @namespace gps_lib
 * @brief A header-only C++ library for parsing and processing NMEA GPS
 * sentences.
 */
namespace gps_lib {
/**
 * @brief Concept that checks if a type is string-like.
 */
template <typename T>
concept StringLike = std::convertible_to<T, std::string_view>;

/**
 * @brief Parses a given NMEA sentence and returns a Sample variant.
 * @param sample  The NMEA sentence to parse.
 * @return std::expected<Sample, ParseError>  An expected containing the parsed
 * Sample or an error.
 * @throws std::invalid_argument If numeric fields are malformed.
 */
inline std::expected<Sample, ParseError> parse(StringLike auto const &sample) {
  if (!gps_lib::is_valid_sample(sample)) {
    return std::unexpected(ParseError::InvalidFormat);
  }

  auto tokens = detail::tokenize(sample);

  if (tokens.empty()) {
    return std::unexpected(ParseError::UnknownError);
  }

  // This won't throw an error because tokens is not empty.
  auto type = tokens.at(0);

  if (type.find("GGA") != std::string::npos) {
    if (tokens.size() < static_cast<size_t>(TokensPerSentence::GGA)) {
      return std::unexpected(ParseError::MissingFields);
    }

    GGA data;

    data.type = tokens.at(0);
    data.utc_time = tokens.at(1);

    try {
      data.latitude.value = std::stod(std::string{tokens.at(2)}) / 100.0;
    } catch (...) {
      return std::unexpected{ParseError::MissingFields};
    }
    if (!tokens.at(3).empty() &&
        (tokens.at(3).front() == 'N' || tokens.at(3).front() == 'S')) {
      data.latitude.direction = tokens.at(3).front();
    } else {
      return std::unexpected{ParseError::InvalidDirection};
    }

    if (!tokens.at(5).empty() &&
        (tokens.at(5).front() == 'E' || tokens.at(5).front() == 'W')) {
      data.longitude.direction = tokens.at(5).front();
    } else {
      return std::unexpected{ParseError::InvalidDirection};
    }
    try {
      double sign = data.longitude.direction == 'W' ? -1.0 : 1.0;
      data.longitude.value =
          std::stod(std::string{tokens.at(4)}) / 100.0 * sign;
    } catch (...) {
      return std::unexpected{ParseError::MissingFields};
    }

    data.quality = tokens.at(6);
    data.satellites_used = tokens.at(7);
    data.hdop = tokens.at(8);
    data.altitude = tokens.at(9);
    data.geoidal_separation = tokens.at(11);
    data.dgps = tokens.at(14);

    return data;
  } else if (type.find("GLL") != std::string::npos) {
    if (tokens.size() < static_cast<size_t>(TokensPerSentence::GLL)) {
      return std::unexpected(ParseError::MissingFields);
    }

    GLL data;

    data.type = tokens.at(0);

    try {
      data.latitude.value = std::stod(std::string{tokens.at(1)}) / 100.0;
    } catch (...) {
      return std::unexpected{ParseError::MissingFields};
    }
    if (!tokens.at(2).empty() &&
        (tokens.at(2).front() == 'N' || tokens.at(2).front() == 'S')) {
      data.latitude.direction = tokens.at(2).front();
    } else {
      return std::unexpected{ParseError::InvalidDirection};
    }

    if (!tokens.at(4).empty() &&
        (tokens.at(4).front() == 'E' || tokens.at(4).front() == 'W')) {
      data.longitude.direction = tokens.at(4).front();
    } else {
      return std::unexpected{ParseError::InvalidDirection};
    }
    try {
      double sign = data.longitude.direction == 'W' ? -1.0 : 1.0;
      data.longitude.value =
          std::stod(std::string{tokens.at(3)}) / 100.0 * sign;
    } catch (...) {
      return std::unexpected{ParseError::MissingFields};
    }

    data.utc_time = tokens.at(6);
    data.status = tokens.at(7);

    return data;
  } else if (type.find("GSA") != std::string::npos) {
    if (tokens.size() < static_cast<size_t>(TokensPerSentence::GSA)) {
      return std::unexpected(ParseError::MissingFields);
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
    if (tokens.size() < static_cast<size_t>(TokensPerSentence::GSV)) {
      return std::unexpected(ParseError::MissingFields);
    }

    GSV data;

    data.type = tokens.at(0);
    data.number_of_messages = tokens.at(1);
    data.sequence_number = tokens.at(2);
    data.satellites_in_view = tokens.at(3);

    try {
      data.satellites.reserve(std::stoi(data.number_of_messages));
    } catch (...) {
      return std::unexpected{ParseError::MissingFields};
    }

    for (int i = 1; i <= std::stoi(data.number_of_messages) &&
                    static_cast<size_t>(i * 4 + 3) < tokens.size();
         ++i) {
      Satellite satellite;

      satellite.id = tokens[i * 4 + 4];
      satellite.elevation = tokens[i * 4 + 5];
      satellite.azimuth = tokens[i * 4 + 6];
      satellite.snr = tokens[i * 4 + 7];

      data.satellites.push_back(satellite);
    }

    return data;
  } else if (type.find("RMC") != std::string::npos) {
    if (tokens.size() < static_cast<size_t>(TokensPerSentence::RMC)) {
      return std::unexpected(ParseError::MissingFields);
    }

    RMC data;

    data.type = tokens.at(0);
    data.utc_time = tokens.at(1);
    data.status = tokens.at(2);

    try {
      data.latitude.value = std::stod(std::string{tokens.at(3)}) / 100.0;
    } catch (...) {
      return std::unexpected{ParseError::MissingFields};
    }
    if (!tokens.at(4).empty() &&
        (tokens.at(4).front() == 'N' || tokens.at(4).front() == 'S')) {
      data.latitude.direction = tokens.at(4).front();
    } else {
      return std::unexpected{ParseError::InvalidDirection};
    }

    if (!tokens.at(6).empty() &&
        (tokens.at(6).front() == 'E' || tokens.at(6).front() == 'W')) {
      data.longitude.direction = tokens.at(6).front();
    } else {
      return std::unexpected{ParseError::InvalidDirection};
    }
    try {
      double sign = data.longitude.direction == 'W' ? -1.0 : 1.0;
      data.longitude.value =
          std::stod(std::string{tokens.at(5)}) / 100.0 * sign;
    } catch (...) {
      return std::unexpected{ParseError::MissingFields};
    }

    data.speed = tokens.at(7);
    data.course = tokens.at(8);
    data.utc_date = tokens.at(9);
    data.mode = tokens.at(11);

    return data;
  } else if (type.find("VTG") != std::string::npos) {
    if (tokens.size() < static_cast<size_t>(TokensPerSentence::VTG)) {
      return std::unexpected(ParseError::MissingFields);
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
    if (tokens.size() < static_cast<size_t>(TokensPerSentence::ZDA)) {
      return std::unexpected(ParseError::MissingFields);
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
    return std::unexpected(ParseError::UnsupportedType);
  }
}
} // namespace gps_lib
