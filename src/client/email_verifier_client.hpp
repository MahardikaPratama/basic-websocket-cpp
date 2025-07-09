#pragma once

#include <string>
#include <memory>
#include "emailverifier.grpc.pb.h"

class EmailVerifierClient {
public:
    explicit EmailVerifierClient(std::shared_ptr<grpc::Channel> channel);

    emailverifier::EmailReply UpdateEmail(const std::string& email);
    emailverifier::DeleteReply DeleteEmail(const std::string& email);

private:
    std::unique_ptr<emailverifier::EmailVerifier::Stub> stub_;
};
