#include <csignal>
#include <iostream>
#include <memory>

#include "grpc_server.h"

grpc::Status AstJsonServiceImpl::SendAstJson(
    grpc::ServerContext*, const AstJson::JsonString* json_str,
    AstJson::Empty*) {
    std::cout << json_str->json_str() << std::endl;

    return grpc::Status::OK;
}

void RunServer(const manager::Config& config) {
    std::string server_address(config.addr_uri);

    AstJsonServiceImpl service;

    grpc::ServerBuilder server_builder;

    server_builder
        .AddListeningPort(server_address, grpc::InsecureServerCredentials())
        .RegisterService(&service);

    std::unique_ptr<grpc::Server> server(server_builder.BuildAndStart());

    std::cout << "Server listening on " << server_address << std::endl;

    auto shutdown_handler = [](int) { abort(); };

    std::signal(SIGTERM, shutdown_handler);

    server->Wait();
}
