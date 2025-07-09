#include <iostream>
#include <string>
#include <limits>
#include <grpc++/grpc++.h>

#include "email_verifier_client.hpp"

void printMenu() {
    std::cout << "\n=== Email Management Menu ===\n";
    std::cout << "1. Check Email\n";
    std::cout << "2. Delete Data Email\n";
    std::cout << "0. Exit\n";
    std::cout << "Choose an option: ";
}

int main() {
    EmailVerifierClient client(
        grpc::CreateChannel("localhost:50051", grpc::InsecureChannelCredentials())
    );

    while (true) {
        printMenu();

        int choice;
        std::cin >> choice;
        std::cin.ignore();

        if (choice == 0) break;

        std::string email;
        std::cout << "Enter email: ";
        std::getline(std::cin, email);

        if (choice == 1) {
            auto result = client.UpdateEmail(email);
            std::cout << "\n[UPDATE RESULT]\n";
            std::cout << " - Valid: " << (result.is_valid() ? "Yes" : "No") << "\n";
            std::cout << " - Domain: " << result.domain() << "\n";
            std::cout << " - Common: " << (result.is_common_domain() ? "Yes" : "No") << "\n";
        } else if (choice == 2) {
            auto result = client.DeleteEmail(email);
            std::cout << "\n[DELETE RESULT]\n";
            std::cout << " - Status: " << result.status() << "\n";
        } else {
            std::cout << "Invalid choice.\n";
        }

        std::cout << "\nPress Enter to continue...";
        std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
    }

    return 0;
}
