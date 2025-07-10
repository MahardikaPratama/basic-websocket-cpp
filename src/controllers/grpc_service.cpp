#include "controllers/grpc_service.hpp"
#include "handlers/email_handler.hpp"
#include "utils/json_utils.hpp"
#include "adapters/websocket_server/websocket_server.hpp"
#include "threads/grpc_websocket_threads.hpp"

grpc::Status EmailVerifierServiceImpl::CheckEmail(grpc::ServerContext*, const email_verifier::EmailRequest* request, email_verifier::EmailReply* reply) {
    ValidateEmail(request->email(), *reply);
    return grpc::Status::OK;
}

grpc::Status EmailVerifierServiceImpl::UpdateEmail(grpc::ServerContext*, const email_verifier::EmailRequest* request, email_verifier::EmailReply* reply) {
    ValidateEmail(request->email(), *reply);
    InsertEmailThreadSafe(request->email());
    broadcast_json(BuildUpdateEmailJson(request->email(), *reply));
    return grpc::Status::OK;
}

grpc::Status EmailVerifierServiceImpl::DeleteEmail(grpc::ServerContext*, const email_verifier::EmailRequest* request, email_verifier::DeleteReply* reply) {
    const std::string& email = request->email();
    bool existed = DeleteEmailThreadSafe(email);
    std::string status = existed ? "deleted" : "not_found";
    reply->set_status(status);
    broadcast_json(BuildDeleteEmailJson(email, status));
    return grpc::Status::OK;
}
