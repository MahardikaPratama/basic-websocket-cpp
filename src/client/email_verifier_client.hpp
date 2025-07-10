#pragma once

#include <string>
#include <memory>
#include "email_verifier.grpc.pb.h"

class EmailVerifierClient {
public:
    explicit EmailVerifierClient(std::shared_ptr<grpc::Channel> channel);

    email_verifier::EmailReply UpdateEmail(const std::string& email);
    email_verifier::DeleteReply DeleteEmail(const std::string& email);

private:
    std::unique_ptr<email_verifier::EmailVerifier::Stub> stub_;
};
