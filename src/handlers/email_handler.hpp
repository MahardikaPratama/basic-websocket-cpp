#pragma once
#include <string>
#include "email_verifier.pb.h"

void ValidateEmail(const std::string& email, email_verifier::EmailReply& reply);
