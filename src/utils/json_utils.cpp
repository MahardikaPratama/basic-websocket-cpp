#include "utils/json_utils.hpp"
#include <rapidjson/document.h>
#include <rapidjson/writer.h>
#include <rapidjson/stringbuffer.h>

std::string BuildUpdateEmailJson(const std::string& email, const email_verifier::EmailReply& reply) {
    rapidjson::Document json;
    json.SetObject();
    auto& a = json.GetAllocator();

    json.AddMember("type", "update", a);
    json.AddMember("email", rapidjson::Value(email.c_str(), a), a);
    json.AddMember("valid", reply.is_valid(), a);
    json.AddMember("domain", rapidjson::Value(reply.domain().c_str(), a), a);
    json.AddMember("common_domain", reply.is_common_domain(), a);

    rapidjson::StringBuffer buffer;
    rapidjson::Writer<rapidjson::StringBuffer> writer(buffer);
    json.Accept(writer);

    return buffer.GetString();
}

std::string BuildDeleteEmailJson(const std::string& email, const std::string& status) {
    rapidjson::Document json;
    json.SetObject();
    auto& a = json.GetAllocator();

    json.AddMember("type", "delete", a);
    json.AddMember("email", rapidjson::Value(email.c_str(), a), a);
    json.AddMember("status", rapidjson::Value(status.c_str(), a), a);

    rapidjson::StringBuffer buffer;
    rapidjson::Writer<rapidjson::StringBuffer> writer(buffer);
    json.Accept(writer);

    return buffer.GetString();
}
