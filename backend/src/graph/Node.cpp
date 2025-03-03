#include "Node.hpp"
#include <cmath>
#include <cpprest/json.h>


using namespace web;

double Coordinates::distanceTo(const Coordinates& other) const {
    const double R = 3440.065; // Earth's radius in nautical miles
    double lat1 = latitude * M_PI / 180.0;
    double lat2 = other.latitude * M_PI / 180.0;
    double dLat = (other.latitude - latitude) * M_PI / 180.0;
    double dLon = (other.longitude - longitude) * M_PI / 180.0;

    double a = sin(dLat/2) * sin(dLat/2) +
               cos(lat1) * cos(lat2) *
               sin(dLon/2) * sin(dLon/2);
    double c = 2 * atan2(sqrt(a), sqrt(1-a));
    return R * c;
}

Node::Node(const std::string& id, const Coordinates& coords, Type type)
    : id(id), coordinates(coords), type(type) {}

web::json::value Node::toJson() const {
    web::json::value json;
    json[U("id")] = web::json::value::string(utility::conversions::to_string_t(id));
    json[U("lat")] = coordinates.latitude;
    json[U("lng")] = coordinates.longitude;
    json[U("type")] = static_cast<int>(type);
    return json;
}

Airport::Airport(const std::string& icao, const std::string& name,
                const std::string& city, const std::string& country,
                int elevation, const Coordinates& coords)
    : Node(icao, coords, Type::AIRPORT),
      name(name), city(city), country(country), elevation(elevation) {}

web::json::value Airport::toJson() const {
    auto json = Node::toJson();
    json[U("name")] = web::json::value::string(utility::conversions::to_string_t(name));
    json[U("city")] = web::json::value::string(utility::conversions::to_string_t(city));
    json[U("country")] = web::json::value::string(utility::conversions::to_string_t(country));
    json[U("elevation")] = elevation;
    return json;
}

Waypoint::Waypoint(const std::string& ident, const std::string& countryCode,
                   const std::string& countryName, const Coordinates& coords)
    : Node(ident, coords, Type::WAYPOINT),
      countryCode(countryCode), countryName(countryName) {}

web::json::value Waypoint::toJson() const {
    auto json = Node::toJson();
    json[U("countryCode")] = web::json::value::string(utility::conversions::to_string_t(countryCode));
    json[U("countryName")] = web::json::value::string(utility::conversions::to_string_t(countryName));
    return json;
}
