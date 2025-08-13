#pragma once

#include <cstddef>
#include <print>
#include <sstream>
#include <string>
#include <variant>
#include <vector>

namespace gps_lib {

enum Units { ms, kmh };

constexpr double KNTOMS{0.514444444};
constexpr double KNTOKMH{1.85};

struct GGA {
  std::string type;
  std::string utc_time;
  std::string latitude;
  std::string latitude_direction;
  std::string longitude;
  std::string longitude_direction;
  std::string quality;
  std::string satellites_used;
  std::string hdop;
  std::string altitude;
  std::string geoidal_separation;
  std::string dgps;
};

struct GLL {
  std::string type;
  std::string latitude;
  std::string latitude_direction;
  std::string longitude;
  std::string longitude_direction;
  std::string utc_time;
  std::string status;
};

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

struct Satellite {
  std::string id;
  std::string elevation;
  std::string azimuth;
  std::string snr;
};

struct GSV {
  std::string type;
  std::string number_of_messages;
  std::string sequence_number;
  std::string satellites_in_view;
  std::vector<Satellite> satellites;
};

struct RMC {
  std::string type;
  std::string utc_time;
  std::string status;
  std::string latitude;
  std::string latitude_direction;
  std::string longitude;
  std::string longitude_direction;
  std::string speed;
  std::string course;
  std::string utc_date;
  std::string mode;
};

struct VTG {
  std::string type;
  std::string course;
  std::string course_magnetic;
  std::string speed_kn;
  std::string speed_kh;
  std::string mode;
};

struct ZDA {
  std::string type;
  std::string utc_time;
  std::string utc_day;
  std::string utc_month;
  std::string utc_year;
  std::string local_zone_hours;
  std::string local_zone_minutes;
};

struct UnknownSample {};

using Sample = std::variant<GGA, GLL, GSA, GSV, RMC, VTG, ZDA, UnknownSample>;

inline std::vector<std::string> split(const std::string &sample,
                                      char separator) {

  size_t start = 0;
  size_t end = 0;

  std::vector<std::string> tokens;

  while ((end = sample.find(separator, start)) != std::string::npos) {
    tokens.push_back(sample.substr(start, end - start));
    start = end + 1;
  }

  tokens.push_back(sample.substr(start));

  return tokens;
}

inline std::vector<std::string> tokenize(const std::string &sample) {
  std::vector<std::string> raw_data = split(sample, '*');
  return split(raw_data.at(0), ',');
}

inline Sample parse(const std::string sample) {
  auto tokens = tokenize(sample);

  auto type = tokens.at(0);

  if (type.find("GGA") != std::string::npos) {
    GGA data;

    data.type = tokens.at(0);
    data.utc_time = tokens.at(1);
    data.latitude = tokens.at(2);
    data.latitude_direction = tokens.at(3);
    data.longitude = tokens.at(4);
    data.longitude_direction = tokens.at(5);
    data.quality = tokens.at(6);
    data.satellites_used = tokens.at(7);
    data.hdop = tokens.at(8);
    data.altitude = tokens.at(9);
    data.geoidal_separation = tokens.at(11);
    data.dgps = tokens.at(14);

    return data;
  } else if (type.find("GLL") != std::string::npos) {
    GLL data;

    data.type = tokens.at(0);
    data.latitude = tokens.at(1);
    data.latitude_direction = tokens.at(2);
    data.longitude = tokens.at(3);
    data.longitude_direction = tokens.at(4);
    data.utc_time = tokens.at(5);
    data.status = tokens.at(6);

    return data;
  } else if (type.find("GSA") != std::string::npos) {
    GSA data;

    data.type = tokens.at(0);
    data.mode = tokens.at(1);
    data.fix_type = tokens.at(2);
    data.pdop = tokens.at(15);
    data.hdop = tokens.at(16);
    data.vdop = tokens.at(17);

    for (int i = 0; i < 12; i++) {
      data.satellites.push_back(tokens[i + 3]);
    }

    return data;
  } else if (type.find("GSV") != std::string::npos) {
    GSV data;

    data.type = tokens.at(0);
    data.number_of_messages = tokens.at(1);
    data.sequence_number = tokens.at(2);
    data.satellites_in_view = tokens.at(3);

    for (int i = 1; i <= std::stoi(data.number_of_messages); i++) {
      Satellite satellite;

      satellite.id = tokens[i * 4 + 0];
      satellite.elevation = tokens[i * 4 + 1];
      satellite.azimuth = tokens[i * 4 + 2];
      satellite.snr = tokens[i * 4 + 3];

      data.satellites.push_back(satellite);
    }

    return data;
  } else if (type.find("RMC") != std::string::npos) {
    RMC data;

    data.type = tokens.at(0);
    data.utc_time = tokens.at(1);
    data.status = tokens.at(2);
    data.latitude = tokens.at(3);
    data.latitude_direction = tokens.at(4);
    data.longitude = tokens.at(5);
    data.longitude_direction = tokens.at(6);
    data.speed = tokens.at(7);
    data.course = tokens.at(8);
    data.utc_date = tokens.at(9);
    data.mode = tokens.at(11);

    return data;
  } else if (type.find("VTG") != std::string::npos) {
    VTG data;

    data.type = tokens.at(0);
    data.course = tokens.at(1);
    data.course_magnetic = tokens.at(3);
    data.speed_kn = tokens.at(5);
    data.speed_kh = tokens.at(7);
    data.mode = tokens.at(9);

    return data;
  } else if (type.find("ZDA") != std::string::npos) {
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
    return UnknownSample{};
  }
}

inline std::tuple<std::string, std::string, std::string>
parse_utc_date(const std::string utc_date) {
  std::string day{utc_date.substr(0, 2)};
  std::string month{utc_date.substr(2, 2)};
  std::string year{utc_date.substr(4, 2)};
  return std::make_tuple(day, month, year);
}

inline std::tuple<std::string, std::string, std::string>
parse_utc_time(const std::string utc_time) {
  std::string hours{utc_time.substr(0, 2)};
  std::string minutes{utc_time.substr(2, 2)};
  std::string seconds{utc_time.substr(4, 2)};
  return std::make_tuple(hours, minutes, seconds);
}

inline double parse_latitude(const std::string latitude) {
  return std::stod(latitude.data()) / 100.0;
}

inline double parse_longitude(const std::string longitude,
                              const std::string longitude_direction) {
  double sign = longitude_direction == "W" ? -1.0 : 1.0;
  return sign * std::stod(longitude.data()) / 100.0;
}

inline double parse_speed(const std::string speed, Units units) {
  double velocity = std::stod(speed.data());
  return units == ms ? velocity * KNTOMS : velocity * KNTOKMH;
}

inline bool is_valid_sample(const std::string sample) {
  std::string segment;
  std::vector<std::string> tokens{split(sample, '*')};

  int check = 0;

  for (size_t i = 0; i < tokens.at(0).size(); i++) {
    check = char(check ^ tokens.at(0).at(i));
  }

  std::stringstream hex_check;

  hex_check << std::uppercase << std::hex << check;

  return hex_check.str().compare(tokens.at(1));
}

inline void print_samples(const Sample &sample) {
  std::visit(
      [](auto &&data) {
        using T = std::decay_t<decltype(data)>;
        if constexpr (std::is_same_v<T, GSA>) {
          std::println("GSA: {}, {}, {}, {}, {}, {}, {}, {}, {}, {}", data.mode,
                       data.fix_type, data.satellites.size(), data.pdop,
                       data.hdop, data.vdop, data.satellites[0],
                       data.satellites[1], data.satellites[2],
                       data.satellites[3]);
        } else if constexpr (std::is_same_v<T, RMC>) {
          std::println("RMC: {}, {}, {}, {}, {}, {}, {}", data.utc_time,
                       data.status, data.latitude, data.latitude_direction,
                       data.longitude, data.longitude_direction, data.speed);
        } else if constexpr (std::is_same_v<T, GGA>) {
          std::println("GGA: {}, {}, {}, {}, {}, {}, {}, {}, {}, {}",
                       data.utc_time, data.latitude, data.latitude_direction,
                       data.longitude, data.longitude_direction, data.quality,
                       data.satellites_used, data.hdop, data.altitude,
                       data.geoidal_separation);
        } else if constexpr (std::is_same_v<T, GLL>) {
          std::println("GLL: {}, {}, {}, {}", data.latitude,
                       data.latitude_direction, data.longitude,
                       data.longitude_direction);
        } else if constexpr (std::is_same_v<T, GSV>) {
          std::println("GSV: {}, {}, {}", data.number_of_messages,
                       data.sequence_number, data.satellites_in_view);
          for (const auto &satellite : data.satellites) {
            std::println(
                "Satellite ID: {}, Elevation: {}, Azimuth: {}, SNR: {}",
                satellite.id, satellite.elevation, satellite.azimuth,
                satellite.snr);
          }
        } else if constexpr (std::is_same_v<T, VTG>) {
          std::println("VTG: {}, {}, {}", data.course, data.speed_kn,
                       data.speed_kh);
        } else if constexpr (std::is_same_v<T, ZDA>) {
          std::println("ZDA: {}, {}, {}, {}", data.utc_time, data.utc_day,
                       data.utc_month, data.utc_year);
        } else {
          std::println("Unknown or unsupported sample type.");
        }
      },
      sample);
}
} // namespace gps_lib
