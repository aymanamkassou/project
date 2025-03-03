// main.cpp
#include "server/Server.hpp"
#include "utils/DataLoader.hpp"
#include <iostream>

int main() {
    try {
        // Load data
        auto waypoints = DataLoader::loadWaypoints("data/waypoints.csv", "MA");
        auto airports = DataLoader::loadAirports("data/airports.json", "MA");
        
        std::cout << "Loaded " << waypoints.size() << " waypoints and "
                  << airports.size() << " airports for Morocco" << std::endl;
        
        // Build graph (connect nodes within 100 nautical miles)
        auto graph = DataLoader::buildGraph(waypoints, airports, 100.0);
        
        // Start server
        Server server("http://localhost:3001", graph);
        server.start();
        
        std::cout << "Press ENTER to exit..." << std::endl;
        std::string line;
        std::getline(std::cin, line);
        
        server.stop();
        return 0;
    }
    catch (const std::exception& e) {
        std::cerr << "Error: " << e.what() << std::endl;
        return 1;
    }
}
