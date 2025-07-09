// email_utils.hpp
#pragma once
#include <string>
#include "emailverifier.pb.h"

bool validate_email(const std::string& email, emailverifier::EmailReply& reply);
