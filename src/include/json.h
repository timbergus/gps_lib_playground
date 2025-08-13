#pragma once

#include <fstream>
#include <nlohmann/json.hpp>

#include "types.h"

/**
 * @namespace gps_lib
 * @brief A header-only C++ library for parsing and processing NMEA GPS
 * sentences.
 */
namespace gps_lib {
/**
 * @brief Serializes a Latitude object to JSON.
 * @param j The JSON object to populate.
 * @param lat The Latitude object to serialize.
 */
inline void to_json(nlohmann::json &j, const Latitude &lat) {
  j = nlohmann::json{{"value", lat.value},
                     {"direction", std::string(1, lat.direction)}};
}

/**
 * @brief Serializes a Longitude object to JSON.
 * @param j The JSON object to populate.
 * @param lon The Longitude object to serialize.
 */
inline void to_json(nlohmann::json &j, const Longitude &lon) {
  j = nlohmann::json{{"value", lon.value},
                     {"direction", std::string(1, lon.direction)}};
}

/**
 * @brief Serializes a Satellite object to JSON.
 * @param j The JSON object to populate.
 * @param sat The Satellite object to serialize.
 */
inline void to_json(nlohmann::json &j, const Satellite &sat) {
  j = nlohmann::json{{"id", sat.id},
                     {"elevation", sat.elevation},
                     {"azimuth", sat.azimuth},
                     {"snr", sat.snr}};
}

/**
 * @brief Serializes a GGA object to JSON.
 * @param j The JSON object to populate.
 * @param gga The GGA object to serialize.
 */
inline void to_json(nlohmann::json &j, const GGA &gga) {
  j = nlohmann::json{{"type", gga.type},
                     {"utc_time", gga.utc_time},
                     {"latitude", gga.latitude},
                     {"longitude", gga.longitude},
                     {"quality", gga.quality},
                     {"satellites_used", gga.satellites_used},
                     {"hdop", gga.hdop},
                     {"altitude", gga.altitude},
                     {"geoidal_separation", gga.geoidal_separation},
                     {"dgps", gga.dgps}};
}

/**
 * @brief Serializes a GLL object to JSON.
 * @param j The JSON object to populate.
 * @param gll The GLL object to serialize.
 */
inline void to_json(nlohmann::json &j, const GLL &gll) {
  j = nlohmann::json{{"type", gll.type},
                     {"latitude", gll.latitude},
                     {"longitude", gll.longitude},
                     {"utc_time", gll.utc_time},
                     {"status", gll.status}};
}

/**
 * @brief Serializes a GSA object to JSON.
 * @param j The JSON object to populate.
 * @param gsa The GSA object to serialize.
 */
inline void to_json(nlohmann::json &j, const GSA &gsa) {
  j = nlohmann::json{{"type", gsa.type},         {"mode", gsa.mode},
                     {"fix_type", gsa.fix_type}, {"satellites", gsa.satellites},
                     {"pdop", gsa.pdop},         {"hdop", gsa.hdop},
                     {"vdop", gsa.vdop},         {"checksum", gsa.checksum}};
}

/**
 * @brief Serializes a GSV object to JSON.
 * @param j The JSON object to populate.
 * @param gsv The GSV object to serialize.
 */
inline void to_json(nlohmann::json &j, const GSV &gsv) {
  j = nlohmann::json{{"type", gsv.type},
                     {"number_of_messages", gsv.number_of_messages},
                     {"sequence_number", gsv.sequence_number},
                     {"satellites_in_view", gsv.satellites_in_view},
                     {"satellites", gsv.satellites}};
}

/**
 * @brief Serializes an RMC object to JSON.
 * @param j The JSON object to populate.
 * @param rmc The RMC object to serialize.
 */
inline void to_json(nlohmann::json &j, const RMC &rmc) {
  j = nlohmann::json{{"type", rmc.type},
                     {"utc_time", rmc.utc_time},
                     {"status", rmc.status},
                     {"latitude", rmc.latitude},
                     {"longitude", rmc.longitude},
                     {"speed", rmc.speed},
                     {"course", rmc.course},
                     {"utc_date", rmc.utc_date},
                     {"mode", rmc.mode}};
}

/**
 * @brief Serializes a VTG object to JSON.
 * @param j The JSON object to populate.
 * @param vtg The VTG object to serialize.
 */
inline void to_json(nlohmann::json &j, const VTG &vtg) {
  j = nlohmann::json{{"type", vtg.type},
                     {"course", vtg.course},
                     {"course_magnetic", vtg.course_magnetic},
                     {"speed_kn", vtg.speed_kn},
                     {"speed_kh", vtg.speed_kh},
                     {"mode", vtg.mode}};
}

/**
 * @brief Serializes a ZDA object to JSON.
 * @param j The JSON object to populate.
 * @param zda The ZDA object to serialize.
 */
inline void to_json(nlohmann::json &j, const ZDA &zda) {
  j = nlohmann::json{{"type", zda.type},
                     {"utc_time", zda.utc_time},
                     {"utc_day", zda.utc_day},
                     {"utc_month", zda.utc_month},
                     {"utc_year", zda.utc_year},
                     {"local_zone_hours", zda.local_zone_hours},
                     {"local_zone_minutes", zda.local_zone_minutes}};
}

/**
 * @brief Serializes a Sample variant to JSON.
 * @param j The JSON object to populate.
 * @param sample The Sample variant to serialize.
 */
inline void to_json(nlohmann::json &j, const Sample &sample) {
  std::visit(
      [&](const auto &value) {
        j = nlohmann::json{{"type", value.type}, {"data", value}};
      },
      sample);
}

/**
 * @brief Saves a Sample to a JSON file.
 * @param sample The Sample to serialize.
 * @param filename The output JSON file path.
 * @return True if the file was written successfully, false otherwise.
 */
inline bool save_to_json(const Sample &sample, const std::string &filename) {
  try {
    nlohmann::json j = sample;
    std::ofstream out(filename);
    if (!out.is_open()) {
      return false;
    }
    out << std::setw(2) << j << std::endl;
    out.close();
    return true;
  } catch (const std::exception &e) {
    std::println("Error saving JSON to {}: {}", filename, e.what());
    return false;
  }
}
} // namespace gps_lib
