#include "adapters/websocket_server/websocket_server.hpp"
#include "globals/constants.hpp"

#include <uWebSockets/App.h>
#include <vector>
#include <mutex>
#include <algorithm>
#include <iostream>

static std::vector<uWS::WebSocket<false, true, int>*> clients;
static std::mutex clients_mutex;

void broadcast_json(const std::string& msg) {
    std::lock_guard<std::mutex> lock(clients_mutex);
    for (auto* ws : clients) {
        ws->send(msg, uWS::OpCode::TEXT);
    }
}

void run_websocket_server() {
    uWS::App::WebSocketBehavior<int> behavior;

    behavior.open = [](auto* ws) {
        std::lock_guard<std::mutex> lock(clients_mutex);
        clients.push_back(ws);
        std::cout << "[WebSocket] Client connected\n";
    };

    behavior.message = [](auto* ws, std::string_view message, uWS::OpCode opCode) {
        std::cout << "[WebSocket] Received (ignored): " << message << std::endl;
        ws->send(kDefaultReplyMessage, opCode);  // pakai konstanta
    };

    behavior.close = [](auto* ws, int, std::string_view) {
        std::lock_guard<std::mutex> lock(clients_mutex);
        clients.erase(std::remove(clients.begin(), clients.end(), ws), clients.end());
        std::cout << "[WebSocket] Client disconnected\n";
    };

    uWS::App()
        .ws<int>("/*", std::move(behavior))
        .listen(kWebSocketPort, [](auto* token) {
            if (token) {
                std::cout << "WebSocket server listening on port " << kWebSocketPort << "\n";
            } else {
                std::cerr << "Failed to start WebSocket server\n";
            }
        })
        .run();
}
