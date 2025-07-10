#pragma once

#include <grpc++/grpc++.h>
#include "email_verifier.grpc.pb.h"

class EmailVerifierServiceImpl final : public email_verifier::EmailVerifier::Service {
public:
    grpc::Status CheckEmail(grpc::ServerContext* context, const email_verifier::EmailRequest* request, email_verifier::EmailReply* reply) override;
    grpc::Status UpdateEmail(grpc::ServerContext* context, const email_verifier::EmailRequest* request, email_verifier::EmailReply* reply) override;
    grpc::Status DeleteEmail(grpc::ServerContext* context, const email_verifier::EmailRequest* request, email_verifier::DeleteReply* reply) override;
};
