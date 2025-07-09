#include <iostream>
#include <string>
#include <limits>

#include <grpc++/grpc++.h>
#include "emailverifier.grpc.pb.h"

using grpc::Channel;
using grpc::ClientContext;
using grpc::Status;

using emailverifier::EmailVerifier;
using emailverifier::EmailRequest;
using emailverifier::EmailReply;
using emailverifier::DeleteReply;

class EmailVerifierClient {
public:
    EmailVerifierClient(std::shared_ptr<Channel> channel)
        : stub_(EmailVerifier::NewStub(channel)) {}

    EmailReply UpdateEmail(const std::string& email) {
        EmailRequest request;
        request.set_email(email);

        EmailReply reply;
        ClientContext context;

        Status status = stub_->UpdateEmail(&context, request, &reply);

        if (!status.ok()) {
            std::cerr << "RPC failed: " << status.error_message()
                      << " (code: " << status.error_code() << ")" << std::endl;
        }

        return reply;
    }

    DeleteReply DeleteEmail(const std::string& email) {
        EmailRequest request;
        request.set_email(email);

        DeleteReply reply;
        ClientContext context;

        Status status = stub_->DeleteEmail(&context, request, &reply);

        if (!status.ok()) {
            std::cerr << "RPC failed: " << status.error_message()
                      << " (code: " << status.error_code() << ")" << std::endl;
        }

        return reply;
    }

private:
    std::unique_ptr<EmailVerifier::Stub> stub_;
};

int main() {
    EmailVerifierClient client(grpc::CreateChannel("localhost:50051", grpc::InsecureChannelCredentials()));

    while (true) {
        std::cout << "\n=== Email Management Menu ===\n";
        std::cout << "1. Update Email\n";
        std::cout << "2. Delete Email\n";
        std::cout << "0. Exit\n";
        std::cout << "Choose an option: ";

        int choice;
        std::cin >> choice;
        std::cin.ignore(); // Clear newline

        if (choice == 0) {
            break;
        }

        std::string input_email;
        std::cout << "Enter email: ";
        std::getline(std::cin, input_email);

        if (choice == 1) {
            EmailReply result = client.UpdateEmail(input_email);

            std::cout << "Pushed to WebSocket clients (UPDATE):\n"
                      << " - Valid: " << (result.is_valid() ? "Yes" : "No") << "\n"
                      << " - Domain: " << result.domain() << "\n"
                      << " - Common Provider: " << (result.is_common_domain() ? "Yes" : "No") << std::endl;

        } else if (choice == 2) {
            DeleteReply result = client.DeleteEmail(input_email);

            std::cout << "Pushed to WebSocket clients (DELETE):\n"
                      << " - Status: " << result.status() << std::endl;

        } else {
            std::cout << "Invalid choice.\n";
        }

        std::cout << "\nPress Enter to continue...";
        std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
    }

    return 0;
}
