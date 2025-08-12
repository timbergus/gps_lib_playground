#include "gps_lib.h"
#include <sstream>

std::any GPS_LIB::parse(const std::string sample) {
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
    return 1;
  }
}

std::vector<std::string> GPS_LIB::tokenize(const std::string sample) {
  std::vector<std::string> rawData = split(sample, '*');
  return split(rawData.at(0), ',');
}

std::vector<std::string> GPS_LIB::split(const std::string sample,
                                        char separator) {
  std::string segment;
  std::vector<std::string> tokens;
  std::stringstream sample_stream{sample.data()};

  while (std::getline(sample_stream, segment, separator)) {
    tokens.push_back(segment);
  }

  return tokens;
}

std::tuple<std::string, std::string, std::string>
GPS_LIB::parse_utc_date(const std::string utc_date) {
  std::string day{utc_date.substr(0, 2)};
  std::string month{utc_date.substr(2, 2)};
  std::string year{utc_date.substr(4, 2)};
  return std::make_tuple(day, month, year);
}

std::tuple<std::string, std::string, std::string>
GPS_LIB::parse_utc_time(const std::string utc_time) {
  std::string hours{utc_time.substr(0, 2)};
  std::string minutes{utc_time.substr(2, 2)};
  std::string seconds{utc_time.substr(4, 2)};
  return std::make_tuple(hours, minutes, seconds);
}

double GPS_LIB::parse_latitude(const std::string latitude) {
  return std::stod(latitude.data()) / 100.0;
}

double GPS_LIB::parse_longitude(const std::string longitude,
                                const std::string longitude_direction) {
  double sign = longitude_direction == "W" ? -1.0 : 1.0;
  return sign * std::stod(longitude.data()) / 100.0;
}

double GPS_LIB::parse_speed(const std::string speed, GPS_LIB::Units units) {
  double velocity = std::stod(speed.data());
  return units == ms ? velocity * KNTOMS : velocity * KNTOKMH;
}

bool GPS_LIB::is_valid_sample(const std::string sample) {
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
