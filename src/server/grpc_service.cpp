// grpc_service.cpp
#include "grpc_service.hpp"
#include "email_utils.hpp"
#include "websocket_server.hpp"
#include <mutex>
#include <set>
#include <iostream>

#include <rapidjson/document.h>
#include <rapidjson/writer.h>
#include <rapidjson/stringbuffer.h>

std::set<std::string> email_storage;
std::mutex storage_mutex;

grpc::Status EmailVerifierServiceImpl::CheckEmail(grpc::ServerContext*, const emailverifier::EmailRequest* request, emailverifier::EmailReply* reply) {
    validate_email(request->email(), *reply);
    return grpc::Status::OK;
}

grpc::Status EmailVerifierServiceImpl::UpdateEmail(grpc::ServerContext*, const emailverifier::EmailRequest* request, emailverifier::EmailReply* reply) {
    validate_email(request->email(), *reply);

    {
        std::lock_guard<std::mutex> lock(storage_mutex);
        email_storage.insert(request->email());
    }

    rapidjson::Document json;
    json.SetObject();
    auto& a = json.GetAllocator();

    json.AddMember("type", rapidjson::Value("update", a), a);
    json.AddMember("email", rapidjson::Value(request->email().c_str(), a), a);
    json.AddMember("valid", reply->is_valid(), a);
    json.AddMember("domain", rapidjson::Value(reply->domain().c_str(), a), a);
    json.AddMember("common_domain", reply->is_common_domain(), a);

    rapidjson::StringBuffer buffer;
    rapidjson::Writer<rapidjson::StringBuffer> writer(buffer);
    json.Accept(writer);

    broadcast_json(buffer.GetString());
    return grpc::Status::OK;
}


grpc::Status EmailVerifierServiceImpl::DeleteEmail(grpc::ServerContext*, const emailverifier::EmailRequest* request, emailverifier::DeleteReply* reply) {
    std::string email = request->email();
    bool existed = false;

    {
        std::lock_guard<std::mutex> lock(storage_mutex);
        existed = email_storage.erase(email) > 0;
    }

    rapidjson::Document json;
    json.SetObject();
    auto& a = json.GetAllocator();

    json.AddMember("type", rapidjson::Value("delete", a), a);
    json.AddMember("email", rapidjson::Value(email.c_str(), a), a);

    if (existed) {
        json.AddMember("status", rapidjson::Value("deleted", a), a);
        reply->set_status("deleted");
    } else {
        json.AddMember("status", rapidjson::Value("not_found", a), a);
        reply->set_status("not_found");
    }

    rapidjson::StringBuffer buffer;
    rapidjson::Writer<rapidjson::StringBuffer> writer(buffer);
    json.Accept(writer);

    broadcast_json(buffer.GetString());
    return grpc::Status::OK;
}
