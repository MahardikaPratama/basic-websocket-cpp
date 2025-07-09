// grpc_service.hpp
#pragma once

#include <grpc++/grpc++.h>
#include "emailverifier.grpc.pb.h"

class EmailVerifierServiceImpl final : public emailverifier::EmailVerifier::Service {
public:
    grpc::Status CheckEmail(grpc::ServerContext* context, const emailverifier::EmailRequest* request, emailverifier::EmailReply* reply) override;
    grpc::Status UpdateEmail(grpc::ServerContext* context, const emailverifier::EmailRequest* request, emailverifier::EmailReply* reply) override;
    grpc::Status DeleteEmail(grpc::ServerContext* context, const emailverifier::EmailRequest* request, emailverifier::DeleteReply* reply) override;
};
