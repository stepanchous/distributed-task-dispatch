#include <spdlog/spdlog.h>

#include "grpc-client/grpc_client.h"

int main(int argc, char* argv[]) {
    if (argc != 2) {
        spdlog::error("Invalid number of arguments");
        return -1;
    }

    RunClient(argv[1]);
}
