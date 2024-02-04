#pragma once

#include <string>

namespace manager {

struct Config {
    static Config FromJson(const std::string& path);

    std::string grpc_address;
    std::string broker_address;
};

}  // namespace manager
