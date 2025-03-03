#include "Server.hpp"
#include <iostream>

Server::Server(const std::string& url, std::shared_ptr<CSRGraph> graph)
    : listener(url), graph(graph) {
    
    listener.support(methods::GET, std::bind(&Server::handleGet, this, std::placeholders::_1));
    listener.support(methods::POST, std::bind(&Server::handlePost, this, std::placeholders::_1));
    listener.support(methods::OPTIONS, std::bind(&Server::handleOptions, this, std::placeholders::_1));
}

void Server::start() {
    try {
        listener.open().wait();
        std::cout << "Server listening at: " << listener.uri().to_string() << std::endl;
    }
    catch (const std::exception& e) {
        std::cerr << "Error starting server: " << e.what() << std::endl;
        throw;
    }
}

void Server::stop() {
    try {
        listener.close().wait();
        std::cout << "Server stopped." << std::endl;
    }
    catch (const std::exception& e) {
        std::cerr << "Error stopping server: " << e.what() << std::endl;
    }
}

void Server::setupCORS(http_request& request) {
    http_response response(status_codes::OK);
    response.headers().add(U("Access-Control-Allow-Origin"), U("*"));
    response.headers().add(U("Access-Control-Allow-Methods"), U("GET, POST, OPTIONS"));
    response.headers().add(U("Access-Control-Allow-Headers"), U("Content-Type"));
    response.headers().add(U("Access-Control-Max-Age"), U("3600"));
    request.reply(response);
}

void Server::handleOptions(http_request request) {
    setupCORS(request);
}

void Server::sendJsonResponse(const http_request& request, const json::value& response) {
    http_response res(status_codes::OK);
    res.headers().add(U("Access-Control-Allow-Origin"), U("*"));
    res.headers().add(U("Content-Type"), U("application/json"));
    res.set_body(response);
    request.reply(res);
}

void Server::sendErrorResponse(const http_request& request, const std::string& error, status_code code) {
    json::value response;
    response[U("error")] = json::value::string(utility::conversions::to_string_t(error));
    
    http_response res(code);
    res.headers().add(U("Access-Control-Allow-Origin"), U("*"));
    res.headers().add(U("Content-Type"), U("application/json"));
    res.set_body(response);
    request.reply(res);
}

void Server::handleGet(http_request request) {
    auto path = request.relative_uri().path();
    
    if (path == U("/api/graph")) {
        getGraphData(request);
    }
    else {
        sendErrorResponse(request, "Endpoint not found", status_codes::NotFound);
    }
}

void Server::handlePost(http_request request) {
    auto path = request.relative_uri().path();
    
    if (path == U("/api/find-path")) {
        findPath(request);
    }
    else {
        sendErrorResponse(request, "Endpoint not found", status_codes::NotFound);
    }
}

void Server::getGraphData(http_request request) {
    try {
        auto graphData = graph->getGraphVisualizationData();
        sendJsonResponse(request, graphData);
    }
    catch (const std::exception& e) {
        sendErrorResponse(request, e.what(), status_codes::InternalError);
    }
}

void Server::findPath(http_request request) {
    try {
        request.extract_json()
        .then([this, request](json::value body) {
            try {
                // Extract parameters
                auto startId = utility::conversions::to_utf8string(body[U("start")].as_string());
                auto endId = utility::conversions::to_utf8string(body[U("end")].as_string());
                auto algorithm = utility::conversions::to_utf8string(body[U("algorithm")].as_string());
                
                // Validate nodes exist
                if (!graph->getNode(startId) || !graph->getNode(endId)) {
                    sendErrorResponse(request, "Invalid start or end node", status_codes::BadRequest);
                    return;
                }
                
                CSRGraph::PathResult result;
                if (algorithm == "dijkstra") {
                    result = graph->findPathDijkstra(startId, endId);
                }
                else if (algorithm == "bfs") {
                    result = graph->findPathBFS(startId, endId);
                }
                else {
                    sendErrorResponse(request, "Invalid algorithm specified", status_codes::BadRequest);
                    return;
                }
                
                sendJsonResponse(request, result.toJson());
            }
            catch (const json::json_exception&) {
                sendErrorResponse(request, "Invalid request body", status_codes::BadRequest);
            }
        })
        .wait();
    }
    catch (const std::exception& e) {
        sendErrorResponse(request, e.what(), status_codes::InternalError);
    }
}
