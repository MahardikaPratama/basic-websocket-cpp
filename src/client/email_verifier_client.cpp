#include "email_verifier_client.hpp"
#include <grpc++/grpc++.h>
#include <iostream>

using grpc::ClientContext;
using grpc::Status;
using emailverifier::EmailRequest;
using emailverifier::EmailReply;
using emailverifier::DeleteReply;

EmailVerifierClient::EmailVerifierClient(std::shared_ptr<grpc::Channel> channel)
    : stub_(emailverifier::EmailVerifier::NewStub(channel)) {}

EmailReply EmailVerifierClient::UpdateEmail(const std::string& email) {
    EmailRequest request;
    request.set_email(email);

    EmailReply reply;
    ClientContext context;

    Status status = stub_->UpdateEmail(&context, request, &reply);

    if (!status.ok()) {
        std::cerr << "[gRPC] UpdateEmail failed: " << status.error_message() << "\n";
    }

    return reply;
}

DeleteReply EmailVerifierClient::DeleteEmail(const std::string& email) {
    EmailRequest request;
    request.set_email(email);

    DeleteReply reply;
    ClientContext context;

    Status status = stub_->DeleteEmail(&context, request, &reply);

    if (!status.ok()) {
        std::cerr << "[gRPC] DeleteEmail failed: " << status.error_message() << "\n";
    }

    return reply;
}
