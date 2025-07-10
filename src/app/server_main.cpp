#include "controllers/grpc_service.hpp"
#include "adapters/websocket_server/websocket_server.hpp"

#include <grpc++/grpc++.h>
#include <thread>
#include <iostream>

void run_grpc_server() {
    std::string address("0.0.0.0:50051");
    EmailVerifierServiceImpl service;

    grpc::ServerBuilder builder;
    builder.AddListeningPort(address, grpc::InsecureServerCredentials());
    builder.RegisterService(&service);
    auto server = builder.BuildAndStart();

    std::cout << "gRPC server listening on " << address << std::endl;
    server->Wait();
}

int main() {
    std::thread grpc_thread(run_grpc_server);
    run_websocket_server();
    grpc_thread.join();
    return 0;
}
