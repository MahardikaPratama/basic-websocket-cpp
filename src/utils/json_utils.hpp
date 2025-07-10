#pragma once
#include <string>
#include "email_verifier.pb.h"

std::string BuildUpdateEmailJson(const std::string& email, const email_verifier::EmailReply& reply);
std::string BuildDeleteEmailJson(const std::string& email, const std::string& status);
