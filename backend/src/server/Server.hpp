#pragma once
#include "../graph/CSRGraph.hpp"
#include <cpprest/http_listener.h>
#include <memory>

using namespace web;
using namespace web::http;
using namespace web::http::experimental::listener;

class Server {
public:
    Server(const std::string& url, std::shared_ptr<CSRGraph> graph);
    
    void start();
    void stop();

private:
    http_listener listener;
    std::shared_ptr<CSRGraph> graph;
    
    // Request handlers
    void handleGet(http_request request);
    void handlePost(http_request request);
    void handleOptions(http_request request);
    
    // Specific endpoint handlers
    void getGraphData(http_request request);
    void findPath(http_request request);
    
    // Helper methods
    void setupCORS(http_request& request);
    void sendJsonResponse(const http_request& request, const json::value& response);
    void sendErrorResponse(const http_request& request, const std::string& error, status_code code);
};


