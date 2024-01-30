#include <iostream>

#include "grpc-client/grpc_client.h"

int main(int argc, char* argv[]) {
    if (argc != 3) {
        std::cerr << "Invalid number of arguments" << std::endl;
    }

    RunClient(client::Config::FromJson(argv[1]), argv[2]);
}
