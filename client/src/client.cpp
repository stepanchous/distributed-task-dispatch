#include <iostream>

#include "grpc-client/grpc_client.h"

int main(int argc, char* argv[]) {
    if (argc != 2) {
        std::cerr << "Invalid number of arguments" << std::endl;
        return -1;
    }

    RunClient(argv[1]);
}
