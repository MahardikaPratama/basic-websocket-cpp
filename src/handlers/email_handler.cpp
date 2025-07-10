#include "handlers/email_handler.hpp"
#include <set>
#include <iostream>

void ValidateEmail(const std::string& email, email_verifier::EmailReply& reply) {
    auto at_pos = email.find('@');
    bool is_valid = (at_pos != std::string::npos) && (at_pos > 0) && (at_pos < email.size() - 1);
    std::string domain = is_valid ? email.substr(at_pos + 1) : "";

    static const std::set<std::string> common_domains = {
        "gmail.com", "yahoo.com", "outlook.com", "hotmail.com", "icloud.com"
    };

    reply.set_is_valid(is_valid);
    reply.set_domain(domain);
    reply.set_is_common_domain(common_domains.count(domain) > 0);

    std::cout << "[EmailHandler] Email: " << email
              << " | Valid: " << is_valid
              << " | Domain: " << domain << std::endl;
}
