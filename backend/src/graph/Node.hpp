#pragma once
#include <string>
#include <vector>
#include <cpprest/json.h>

using namespace web;

struct Coordinates {
    double latitude;
    double longitude;

    // Calculate distance between two coordinates (in nautical miles)
    double distanceTo(const Coordinates& other) const;
};

class Node {
public:
    enum class Type {
        AIRPORT,
        WAYPOINT
    };

    Node(const std::string& id, const Coordinates& coords, Type type);
    
    std::string getId() const { return id; }
    Coordinates getCoordinates() const { return coordinates; }
    Type getType() const { return type; }
    
    // Serialization for JSON responses
    virtual web::json::value toJson() const;

private:
    std::string id;
    Coordinates coordinates;
    Type type;
};

class Airport : public Node {
public:
    Airport(const std::string& icao, const std::string& name, 
           const std::string& city, const std::string& country,
           int elevation, const Coordinates& coords);

    std::string getName() const { return name; }
    std::string getCity() const { return city; }
    std::string getCountry() const { return country; }
    int getElevation() const { return elevation; }

    web::json::value toJson() const override;

private:
    std::string name;
    std::string city;
    std::string country;
    int elevation;
};

class Waypoint : public Node {
public:
    Waypoint(const std::string& ident, const std::string& countryCode,
             const std::string& countryName, const Coordinates& coords);

    std::string getCountryCode() const { return countryCode; }
    std::string getCountryName() const { return countryName; }

    web::json::value toJson() const override;

private:
    std::string countryCode;
    std::string countryName;
};
