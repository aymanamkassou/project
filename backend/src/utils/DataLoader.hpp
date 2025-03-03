#pragma once
#include "../graph/Node.hpp"
#include "../graph/CSRGraph.hpp"
#include <memory>
#include <string>
#include <vector>

class DataLoader {
public:
    // Load waypoints from CSV file
    static std::vector<std::shared_ptr<Waypoint>> loadWaypoints(const std::string& filename, 
                                                               const std::string& countryCode);
    
    // Load airports from JSON file
    static std::vector<std::shared_ptr<Airport>> loadAirports(const std::string& filename, 
                                                             const std::string& countryCode);
    
    // Build graph from loaded data
    static std::shared_ptr<CSRGraph> buildGraph(const std::vector<std::shared_ptr<Waypoint>>& waypoints,
                                              const std::vector<std::shared_ptr<Airport>>& airports,
                                              double maxDistance);

private:
    static std::vector<std::string> splitCSV(const std::string& line);
};

