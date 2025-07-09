#include <iostream>
#include <sstream>
#include <string>
#include <set>
#include <thread>
#include <mutex>
#include <vector>
#include <algorithm>

#include <grpc++/grpc++.h>
#include <uWebSockets/App.h>

#include "emailverifier.grpc.pb.h"

#include <rapidjson/document.h>
#include <rapidjson/writer.h>
#include <rapidjson/stringbuffer.h>

using grpc::Server;
using grpc::ServerBuilder;
using grpc::ServerContext;
using grpc::Status;
using emailverifier::EmailVerifier;
using emailverifier::EmailRequest;
using emailverifier::EmailReply;
using emailverifier::DeleteReply;

std::vector<uWS::WebSocket<false, true, int>*> ws_clients;
std::mutex ws_mutex;

std::set<std::string> email_storage;
std::mutex storage_mutex;

void broadcast_json(const std::string& json_msg) {
    std::lock_guard<std::mutex> lock(ws_mutex);
    for (auto* client : ws_clients) {
        client->send(json_msg, uWS::OpCode::TEXT);
    }
}

class EmailVerifierServiceImpl final : public EmailVerifier::Service {
public:
    Status CheckEmail(ServerContext* context, const EmailRequest* request, EmailReply* reply) override {
        return validate_and_fill_reply(request->email(), reply);
    }

    Status UpdateEmail(ServerContext* context, const EmailRequest* request, EmailReply* reply) override {
        validate_and_fill_reply(request->email(), reply);

        {
            std::lock_guard<std::mutex> lock(storage_mutex);
            email_storage.insert(request->email());
        }

        // Create JSON payload
        rapidjson::Document jsonDoc;
        jsonDoc.SetObject();
        auto& allocator = jsonDoc.GetAllocator();

        jsonDoc.AddMember("type", "update", allocator);
        jsonDoc.AddMember("email", rapidjson::Value().SetString(request->email().c_str(), allocator), allocator);
        jsonDoc.AddMember("valid", reply->is_valid(), allocator);
        jsonDoc.AddMember("domain", rapidjson::Value().SetString(reply->domain().c_str(), allocator), allocator);
        jsonDoc.AddMember("common_domain", reply->is_common_domain(), allocator);

        rapidjson::StringBuffer buffer;
        rapidjson::Writer<rapidjson::StringBuffer> writer(buffer);
        jsonDoc.Accept(writer);

        broadcast_json(buffer.GetString());
        return Status::OK;
    }

    Status DeleteEmail(ServerContext* context, const EmailRequest* request, DeleteReply* reply) override {
        std::string email = request->email();
        bool existed = false;

        {
            std::lock_guard<std::mutex> lock(storage_mutex);
            existed = email_storage.erase(email) > 0;
        }

        rapidjson::Document jsonDoc;
        jsonDoc.SetObject();
        auto& allocator = jsonDoc.GetAllocator();

        jsonDoc.AddMember("type", "delete", allocator);
        jsonDoc.AddMember("email", rapidjson::Value().SetString(email.c_str(), allocator), allocator);

        if (existed) {
            jsonDoc.AddMember("status", "deleted", allocator);
            reply->set_status("deleted");
            std::cout << "[gRPC] Deleted: " << email << std::endl;
        } else {
            jsonDoc.AddMember("status", "not_found", allocator);
            reply->set_status("not_found");
            std::cout << "[gRPC] Delete failed (not found): " << email << std::endl;
        }

        rapidjson::StringBuffer buffer;
        rapidjson::Writer<rapidjson::StringBuffer> writer(buffer);
        jsonDoc.Accept(writer);

        broadcast_json(buffer.GetString());
        return Status::OK;
    }

private:
    Status validate_and_fill_reply(const std::string& email, EmailReply* reply) {
        auto at_pos = email.find('@');
        bool is_valid = (at_pos != std::string::npos) && (at_pos > 0) && (at_pos < email.size() - 1);
        std::string domain = is_valid ? email.substr(at_pos + 1) : "";

        static const std::set<std::string> common_domains = {
            "gmail.com", "yahoo.com", "outlook.com", "hotmail.com", "icloud.com"
        };

        reply->set_is_valid(is_valid);
        reply->set_domain(domain);
        reply->set_is_common_domain(common_domains.count(domain) > 0);

        std::cout << "[gRPC] Email: " << email
                  << " | Valid: " << is_valid
                  << " | Domain: " << domain << std::endl;

        return Status::OK;
    }
};

void RunGrpcServer() {
    std::string server_address("0.0.0.0:50051");
    EmailVerifierServiceImpl service;

    ServerBuilder builder;
    builder.AddListeningPort(server_address, grpc::InsecureServerCredentials());
    builder.RegisterService(&service);
    std::unique_ptr<Server> server(builder.BuildAndStart());

    std::cout << "gRPC Server running at " << server_address << std::endl;
    server->Wait();
}

int main() {
    std::thread grpc_thread(RunGrpcServer);

    uWS::App::WebSocketBehavior<int> behavior;
    behavior.open = [](auto* ws) {
        std::lock_guard<std::mutex> lock(ws_mutex);
        ws_clients.push_back(ws);
        std::cout << "[WebSocket] Client connected" << std::endl;
    };

    behavior.message = [](auto* ws, std::string_view message, uWS::OpCode opCode) {
        std::cout << "[WebSocket] Received (ignored): " << message << std::endl;
        ws->send("{\"info\":\"Use gRPC to insert, update, or delete data\"}", opCode);
    };

    behavior.close = [](auto* ws, int code, std::string_view msg) {
        std::lock_guard<std::mutex> lock(ws_mutex);
        ws_clients.erase(std::remove(ws_clients.begin(), ws_clients.end(), ws), ws_clients.end());
        std::cout << "[WebSocket] Client disconnected" << std::endl;
    };

    uWS::App().ws<int>("/*", std::move(behavior))
        .listen(9001, [](auto* token) {
            if (token) {
                std::cout << "WebSocket server listening on port 9001" << std::endl;
            } else {
                std::cerr << "Failed to start WebSocket server" << std::endl;
            }
        })
        .run();

    grpc_thread.join();
    return 0;
}
