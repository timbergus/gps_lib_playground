#pragma once

#include <any>
#include <map>
#include <string>
#include <vector>
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

class GPS_LIB {
public:
  enum Units { ms, kmh };

  static std::any parse(const std::string sample);

  // Tools

  static std::vector<std::string> split(const std::string, char);
  static std::string stringify_vector(std::vector<std::string>);
  static std::string stringify_map(std::map<std::string, std::string>);
  static std::tuple<std::string, std::string, std::string>
  parse_utc_date(const std::string);
  static std::tuple<std::string, std::string, std::string>
  parse_utc_time(const std::string);
  static double parse_latitude(const std::string);
  static double parse_longitude(const std::string, const std::string);
  static double parse_speed(const std::string, Units);
  static bool is_valid_sample(const std::string);

private:
  static constexpr double KNTOMS{0.514444444};
  static constexpr double KNTOKMH{1.85};

  static std::vector<std::string> tokenize(const std::string sample);
};
