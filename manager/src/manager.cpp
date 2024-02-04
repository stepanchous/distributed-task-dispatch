#include <iostream>

#include "grpc-server/grpc_server.h"
#include "manager-config/manager_config.h"

int main(int argc, char* argv[]) {
    if (argc != 2) {
        std::cerr << "Invalid number of arguments" << std::endl;
        return -1;
    }

    RunServer(manager::Config::FromJson(argv[1]));
}
