// CSRGraph.cpp
#include "CSRGraph.hpp"
#include <queue>
#include <limits>
#include <unordered_set>

web::json::value CSRGraph::AlgorithmStep::toJson() const {
    web::json::value json;
    json[U("currentNode")] = web::json::value::string(utility::conversions::to_string_t(currentNode));
    
    web::json::value visitedJson = web::json::value::array();
    for (size_t i = 0; i < visitedNodes.size(); ++i) {
        visitedJson[i] = web::json::value::string(utility::conversions::to_string_t(visitedNodes[i]));
    }
    json[U("visitedNodes")] = visitedJson;
    
    web::json::value frontierJson = web::json::value::array();
    for (size_t i = 0; i < frontier.size(); ++i) {
        frontierJson[i] = web::json::value::string(utility::conversions::to_string_t(frontier[i]));
    }
    json[U("frontier")] = frontierJson;
    
    web::json::value distancesJson = web::json::value::object();
    for (const auto& pair : distances) {
      if (pair.second == std::numeric_limits<double>::infinity()) {
        distancesJson[utility::conversions::to_string_t(pair.first)] = web::json::value::string(U("∞"));
      } else {
       distancesJson[utility::conversions::to_string_t(pair.first)] = json::value::number(pair.second);
      }
    } 
    json[U("distances")] = distancesJson;
    
    web::json::value previousJson = web::json::value::object();
    for (const auto& pair : previousNodes) {
        previousJson[utility::conversions::to_string_t(pair.first)] = 
            web::json::value::string(utility::conversions::to_string_t(pair.second));
    }
    json[U("previousNodes")] = previousJson;
    
    return json;
}

web::json::value CSRGraph::PathResult::toJson() const {
    web::json::value json;
    
    web::json::value pathJson = web::json::value::array();
    for (size_t i = 0; i < path.size(); ++i) {
        pathJson[i] = web::json::value::string(utility::conversions::to_string_t(path[i]));
    }
    json[U("path")] = pathJson;
    
    json[U("totalDistance")] = totalDistance;
    
    web::json::value stepsJson = web::json::value::array();
    for (size_t i = 0; i < steps.size(); ++i) {
        stepsJson[i] = steps[i].toJson();
    }
    json[U("steps")] = stepsJson;
    
    return json;
}

void CSRGraph::addNode(std::shared_ptr<Node> node) {
    nodeIndices[node->getId()] = nodes.size();
    nodes.push_back(node);
    rowPtr.push_back(colIdx.size());
}

void CSRGraph::addEdge(int from, int to, double weight) {
    colIdx.push_back(to);
    values.push_back(weight);
}

void CSRGraph::connectNodesWithinRange(double maxDistance) {
    rowPtr.clear();
    colIdx.clear();
    values.clear();
    rowPtr.push_back(0);
    
    for (size_t i = 0; i < nodes.size(); ++i) {
        for (size_t j = 0; j < nodes.size(); ++j) {
            if (i != j) {
                double distance = nodes[i]->getCoordinates().distanceTo(nodes[j]->getCoordinates());
                if (distance <= maxDistance) {
                    addEdge(i, j, distance);
                }
            }
        }
        rowPtr.push_back(colIdx.size());
    }
}

web::json::value CSRGraph::getGraphVisualizationData() const {
    web::json::value json;
    
    // Add nodes
    web::json::value nodesJson = web::json::value::array();
    for (size_t i = 0; i < nodes.size(); ++i) {
        nodesJson[i] = nodes[i]->toJson();
    }
    json[U("nodes")] = nodesJson;
    
    // Add edges
    web::json::value edgesJson = web::json::value::array();
    int edgeIndex = 0;
    for (size_t i = 0; i < nodes.size(); ++i) {
        for (int j = rowPtr[i]; j < rowPtr[i + 1]; ++j) {
            web::json::value edge;
            edge[U("from")] = web::json::value::string(utility::conversions::to_string_t(nodes[i]->getId()));
            edge[U("to")] = web::json::value::string(utility::conversions::to_string_t(nodes[colIdx[j]]->getId()));
            edge[U("distance")] = values[j];
            edgesJson[edgeIndex++] = edge;
        }
    }
    json[U("edges")] = edgesJson;
    
    return json;
}

bool isAirport(const std::string& id) {
    // For Moroccan airports
    if (id.length() == 4 && id.substr(0, 2) == "GM") {
        return true;
    }
    return false;
}

// Implementation of Dijkstra's algorithm with step tracking
CSRGraph::PathResult CSRGraph::findPathDijkstra(const std::string& start, const std::string& end) {
    PathResult result;
    std::unordered_map<std::string, double> distances;
    std::unordered_map<std::string, std::string> previous;
    std::unordered_set<std::string> visited;

    if (!isAirport(start) || !isAirport(end)) {
        return result;
    }

    
    // Initialize distances
    for (const auto& node : nodes) {
        distances[node->getId()] = std::numeric_limits<double>::infinity();
    }
    distances[start] = 0;
    
    // Priority queue for Dijkstra's algorithm
    auto compare = [](const std::pair<std::string, double>& a, const std::pair<std::string, double>& b) {
        return a.second > b.second;
    };
    std::priority_queue<std::pair<std::string, double>, 
                       std::vector<std::pair<std::string, double>>,
                       decltype(compare)> pq(compare);
    
    pq.push({start, 0});

    
    while (!pq.empty()) {

        std::string current = pq.top().first;
        pq.pop();
        
        if (visited.count(current)) continue;
        visited.insert(current);
        
        AlgorithmStep step;
        step.currentNode = current;
        step.visitedNodes = std::vector<std::string>(visited.begin(), visited.end());
        step.distances = distances;
        step.previousNodes = previous;
        
        // Get frontier nodes
        std::vector<std::string> frontier;
        int nodeIdx = nodeIndices[current];
        for (int i = rowPtr[nodeIdx]; i < rowPtr[nodeIdx + 1]; ++i) {
            std::string neighborId = nodes[colIdx[i]]->getId();
            if (isAirport(neighborId) && neighborId != end) continue;
            if (!visited.count(neighborId)) {
                frontier.push_back(neighborId);
            }
        }
        step.frontier = frontier;
        result.steps.push_back(step);
        
        if (current == end) break;
        
        // Process neighbors
        nodeIdx = nodeIndices[current];
        for (int i = rowPtr[nodeIdx]; i < rowPtr[nodeIdx + 1]; ++i) {
            std::string neighborId = nodes[colIdx[i]]->getId();
            if (isAirport(neighborId) && neighborId != end) continue;
            double distance = distances[current] + values[i];
            
            if (distance < distances[neighborId]) {
                distances[neighborId] = distance;
                previous[neighborId] = current;
                pq.push({neighborId, distance});
            }
        }
    }
    
    // Reconstruct path
    if (distances[end] != std::numeric_limits<double>::infinity()) {
        std::string current = end;
        while (current != start) {
            result.path.push_back(current);
            current = previous[current];
        }
        result.path.push_back(start);
        std::reverse(result.path.begin(), result.path.end());
        result.totalDistance = distances[end];
    }
    
    return result;
}

// Implementation of BFS with step tracking
CSRGraph::PathResult CSRGraph::findPathBFS(const std::string& start, const std::string& end) {
    PathResult result;
    std::unordered_map<std::string, std::string> previous;
    std::unordered_set<std::string> visited;
    std::queue<std::string> queue;
    
    queue.push(start);
    visited.insert(start);
    
    while (!queue.empty()) {
        std::string current = queue.front();
        queue.pop();
        
        // Create algorithm step
        AlgorithmStep step;
        step.currentNode = current;
        step.visitedNodes = std::vector<std::string>(visited.begin(), visited.end());
        step.previousNodes = previous;
        
        // Get frontier nodes
        std::vector<std::string> frontier;
        int nodeIdx = nodeIndices[current];
        for (int i = rowPtr[nodeIdx]; i < rowPtr[nodeIdx + 1]; ++i) {
            std::string neighborId = nodes[colIdx[i]]->getId();
            if (isAirport(neighborId) && neighborId != end) continue;
            if (!visited.count(neighborId)) {
                frontier.push_back(neighborId);
            }
        }
        step.frontier = frontier;
        
        // Add distances for visualization (even though BFS doesn't use them)
        std::unordered_map<std::string, double> distances;
        for (const auto& node : nodes) {
            distances[node->getId()] = visited.count(node->getId()) ? 1.0 : std::numeric_limits<double>::infinity();
        }
        step.distances = distances;
        
        result.steps.push_back(step);
        
        if (current == end) break;
        
        // Process neighbors
        nodeIdx = nodeIndices[current];
        for (int i = rowPtr[nodeIdx]; i < rowPtr[nodeIdx + 1]; ++i) {
            std::string neighborId = nodes[colIdx[i]]->getId();
            if (isAirport(neighborId) && neighborId != end) continue;
            if (!visited.count(neighborId)) {
                visited.insert(neighborId);
                previous[neighborId] = current;
                queue.push(neighborId);
            }
        }
    }
    
    // Reconstruct path
    if (visited.count(end)) {
        std::string current = end;
        double totalDistance = 0;
        
        while (current != start) {
            result.path.push_back(current);
            std::string prev = previous[current];
            
            // Calculate actual distance for this edge
            int nodeIdx = nodeIndices[prev];
            for (int i = rowPtr[nodeIdx]; i < rowPtr[nodeIdx + 1]; ++i) {
                if (nodes[colIdx[i]]->getId() == current) {
                    totalDistance += values[i];
                    break;
                }
            }
            
            current = prev;
        }
        result.path.push_back(start);
        std::reverse(result.path.begin(), result.path.end());
        result.totalDistance = totalDistance;
    }
    
    return result;
}

std::shared_ptr<Node> CSRGraph::getNode(const std::string& id) const {
    auto it = nodeIndices.find(id);
    if (it != nodeIndices.end()) {
        return nodes[it->second];
    }
    return nullptr;
}
