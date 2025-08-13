#pragma once

#include <string>
#include <variant>
#include <vector>

/**
 * @namespace gps_lib
 * @brief A header-only C++ library for parsing and processing NMEA GPS
 * sentences.
 */
namespace gps_lib {
/**
 * @brief This enum represents the speed units in meters per second and
 * kilometers per hour.
 */
enum class Units {
  ms, ///< Meters per second
  kmh ///< Kilometers per hour
};

/**
 * @brief This enum represents the number of tokens for each NMEA sentence type.
 */
enum class TokensPerSentence {
  GGA = 15, ///< Number of tokens for GGA sentence
  GLL = 7,  ///< Number of tokens for GLL sentence
  GSA = 18, ///< Number of tokens for GSA sentence
  GSV = 4,  ///< Number of tokens for GSV sentence
  RMC = 12, ///< Number of tokens for RMC sentence
  VTG = 10, ///< Number of tokens for VTG sentence
  ZDA = 7   ///< Number of tokens for ZDA sentence
};

/**
 * @brief This constant represents the conversion factor from knots to meters
 * per second.
 */
constexpr double KNTOMS{0.514444444};

/**
 * @brief This constant represents the conversion factor from knots to
 * kilometers per hour.
 */
constexpr double KNTOKMH{1.85};

/**
 * @brief Stores the latitude value in decimal degrees and direction ('N' or
 * 'S').
 */
struct Latitude {
  double value;   ///< Latitude in decimal degrees.
  char direction; ///< Direction of latitude ('N' or 'S').
};

/**
 * @brief This struct represents the longitude in a GPS coordinate.
 */
struct Longitude {
  double value;   ///< Longitude in decimal degrees.
  char direction; ///< Direction of longitude ('E' or 'W').
};

/**
 * @brief This struct represents the GGA (Global Positioning System Fix Data)
 * sentence.
 */
struct GGA {
  std::string type;     ///< Type of the NMEA sentence (GGA).
  std::string utc_time; ///< UTC time in the format HHMMSS.
  Latitude
      latitude; ///< Latitude in decimal degrees and direction ('N' or 'S').
  Longitude
      longitude; ///< Longitude in decimal degrees and direction ('E' or 'W').
  std::string quality;            ///< GPS fix quality indicator.
  std::string satellites_used;    ///< Number of satellites used for the fix.
  std::string hdop;               ///< Horizontal dilution of precision.
  std::string altitude;           ///< Altitude in meters.
  std::string geoidal_separation; ///< Geoidal separation in meters.
  std::string dgps;               ///< Differential GPS data.
};

/**
 * @brief This struct represents the GLL (Geographic Latitude and Longitude)
 * sentence.
 */
struct GLL {
  std::string type; ///< Type of the NMEA sentence (GLL).
  Latitude
      latitude; ///< Latitude in decimal degrees and direction ('N' or 'S').
  Longitude
      longitude; ///< Longitude in decimal degrees and direction ('E' or 'W').
  std::string utc_time; ///< UTC time in the format HHMMSS.
  std::string status;   ///< Status of the fix ('A' for active, 'V' for void).
};

/**
 * @brief This struct represents the GSA (GNSS DOP and Active Satellites)
 * sentence.
 */
struct GSA {
  std::string type;     ///< Type of the NMEA sentence (GSA).
  std::string mode;     ///< Mode (1 = no fix, 2 = 2D fix, 3 = 3D fix).
  std::string fix_type; ///< Fix type (0 = no fix, 1 = GPS fix, 2 = DGPS fix).
  std::vector<std::string> satellites; ///< List of satellites used for the fix.
  std::string pdop;                    ///< Position dilution of precision.
  std::string hdop;                    ///< Horizontal dilution of precision.
  std::string vdop;                    ///< Vertical dilution of precision.
  std::string checksum;                ///< Checksum for the sentence.
};

/**
 * @brief This struct represents a satellite in the GPS system.
 */
struct Satellite {
  std::string id;        ///< Satellite ID.
  std::string elevation; ///< Satellite elevation in degrees.
  std::string azimuth;   ///< Satellite azimuth in degrees.
  std::string snr;       ///< Satellite signal-to-noise ratio.
};

/**
 * @brief This struct represents the GSV (GNSS Satellites in View) sentence.
 */
struct GSV {
  std::string type;                  ///< Type of the NMEA sentence (GSV).
  std::string number_of_messages;    ///< Total number of messages.
  std::string sequence_number;       ///< Sequence number of this message.
  std::string satellites_in_view;    ///< Number of satellites in view.
  std::vector<Satellite> satellites; ///< List of satellites.
};

/**
 * @brief This struct represents the RMC (Recommended Minimum Specific
 * GPS/Transit Data) sentence.
 */
struct RMC {
  std::string type;     ///< Type of the NMEA sentence (RMC).
  std::string utc_time; ///< UTC time in the format HHMMSS.
  std::string status;   ///< Status of the fix ('A' for active, 'V' for void).
  Latitude
      latitude; ///< Latitude in decimal degrees and direction ('N' or 'S').
  Longitude
      longitude; ///< Longitude in decimal degrees and direction ('E' or 'W').
  std::string speed;    ///< Speed over ground in knots.
  std::string course;   ///< Course over ground in degrees.
  std::string utc_date; ///< UTC date in the format DDMMYY.
  std::string mode; ///< Mode (A = autonomous, D = differential, E = estimated).
};

/**
 * @brief This struct represents the VTG (Course Over Ground and Ground Speed)
 * sentence.
 */
struct VTG {
  std::string type;            ///< Type of the NMEA sentence (VTG).
  std::string course;          ///< Course over ground in degrees.
  std::string course_magnetic; ///< Magnetic course in degrees.
  std::string speed_kn;        ///< Speed over ground in knots.
  std::string speed_kh;        ///< Speed over ground in kilometers per hour.
  std::string mode; ///< Mode (A = autonomous, D = differential, E = estimated).
};

/**
 * @brief This struct represents the ZDA (Time and Date) sentence.
 */
struct ZDA {
  std::string type;               ///< Type of the NMEA sentence (ZDA).
  std::string utc_time;           ///< UTC time in the format HHMMSS.
  std::string utc_day;            ///< UTC day.
  std::string utc_month;          ///< UTC month.
  std::string utc_year;           ///< UTC year.
  std::string local_zone_hours;   ///< Local zone hours.
  std::string local_zone_minutes; ///< Local zone minutes.
};

/**
 * @brief This enum represents the various parsing errors that can occur.
 */
enum class ParseError {
  InvalidDirection, ///< Invalid direction in latitude or longitude.
  InvalidFormat,    ///< Invalid format of the NMEA sentence.
  MissingFields,    ///< Missing fields in the NMEA sentence.
  UnknownError,     ///< An unknown error occurred.
  UnsupportedType,  ///< The NMEA sentence type is not supported.
};

/**
 * @brief This variant represents a sample NMEA sentence.
 */
using Sample = std::variant<GGA, GLL, GSA, GSV, RMC, VTG, ZDA>;
} // namespace gps_lib
