#include "DataLoader.hpp"
#include <fstream>
#include <sstream>
#include <cpprest/json.h>

std::vector<std::string> DataLoader::splitCSV(const std::string& line) {
    std::vector<std::string> result;
    std::stringstream ss(line);
    std::string item;
    
    while (std::getline(ss, item, ',')) {
        // Remove quotes if present
        if (item.size() >= 2 && item.front() == '"' && item.back() == '"') {
            item = item.substr(1, item.size() - 2);
        }
        result.push_back(item);
    }
    
    return result;
}

std::vector<std::shared_ptr<Waypoint>> DataLoader::loadWaypoints(
    const std::string& filename, const std::string& countryCode) {
    
    std::vector<std::shared_ptr<Waypoint>> waypoints;
    std::ifstream file(filename);
    std::string line;
    
    // Skip header
    std::getline(file, line);
    
    while (std::getline(file, line)) {
        auto parts = splitCSV(line);
        if (parts.size() >= 5 && parts[0] == countryCode) {
            Coordinates coords{std::stod(parts[3]), std::stod(parts[4])};
            waypoints.push_back(std::make_shared<Waypoint>(
                parts[2], parts[0], parts[1], coords
            ));
        }
    }
    
    return waypoints;
}

std::vector<std::shared_ptr<Airport>> DataLoader::loadAirports(
    const std::string& filename, const std::string& countryCode) {
    
    std::vector<std::shared_ptr<Airport>> airports;
    std::ifstream file(filename);
    std::string jsonStr((std::istreambuf_iterator<char>(file)),
                        std::istreambuf_iterator<char>());
    
    web::json::value jsonData = web::json::value::parse(jsonStr);
    
    for (const auto& pair : jsonData.as_object()) {
        const auto& airportData = pair.second;
        std::string country = utility::conversions::to_utf8string(airportData.at(U("country")).as_string());
        
        if (country == countryCode) {
            Coordinates coords{
                airportData.at(U("lat")).as_double(),
                airportData.at(U("lon")).as_double()
            };
            
            airports.push_back(std::make_shared<Airport>(
                utility::conversions::to_utf8string(pair.first),
                utility::conversions::to_utf8string(airportData.at(U("name")).as_string()),
                utility::conversions::to_utf8string(airportData.at(U("city")).as_string()),
                country,
                airportData.at(U("elevation")).as_integer(),
                coords
            ));
        }
    }
    
    return airports;
}

std::shared_ptr<CSRGraph> DataLoader::buildGraph(
    const std::vector<std::shared_ptr<Waypoint>>& waypoints,
    const std::vector<std::shared_ptr<Airport>>& airports,
    double maxDistance) {
    
    auto graph = std::make_shared<CSRGraph>();
    
    // Add all nodes to the graph
    for (const auto& waypoint : waypoints) {
        graph->addNode(waypoint);
    }
    for (const auto& airport : airports) {
        graph->addNode(airport);
    }
    
    // Connect nodes within range
    graph->connectNodesWithinRange(maxDistance);
    
    return graph;
}
