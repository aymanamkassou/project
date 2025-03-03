#pragma once
#include "Node.hpp"
#include <memory>
#include <unordered_map>
#include <vector>

class CSRGraph {
public:
    // Structure to hold algorithm step information for visualization
    struct AlgorithmStep {
        std::string currentNode;
        std::vector<std::string> visitedNodes;
        std::vector<std::string> frontier;
        std::unordered_map<std::string, double> distances;
        std::unordered_map<std::string, std::string> previousNodes;
        
        web::json::value toJson() const;
    };

    // Structure to hold path finding results
    struct PathResult {
        std::vector<std::string> path;
        double totalDistance;
        std::vector<AlgorithmStep> steps;
        
        web::json::value toJson() const;
    };

    // Add a node to the graph
    void addNode(std::shared_ptr<Node> node);
    
    // Connect nodes within specified range (in nautical miles)
    void connectNodesWithinRange(double maxDistance);
    
    // Get nodes and edges for visualization
    web::json::value getGraphVisualizationData() const;
    
    // Path finding algorithms
    PathResult findPathDijkstra(const std::string& start, const std::string& end);
    PathResult findPathBFS(const std::string& start, const std::string& end);
    
    // Get node by ID
    std::shared_ptr<Node> getNode(const std::string& id) const;

private:
    std::vector<std::shared_ptr<Node>> nodes;
    std::vector<int> rowPtr;
    std::vector<int> colIdx;
    std::vector<double> values;
    std::unordered_map<std::string, int> nodeIndices;
    
    // Helper method to add an edge
    void addEdge(int from, int to, double weight);
};

