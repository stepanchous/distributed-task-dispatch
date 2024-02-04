#include <fstream>
#include <nlohmann/json.hpp>
#include <stdexcept>

#include "manager_config.h"

namespace manager {

constexpr std::string GRPC_ADDRESS = "grpc_address";
constexpr std::string BROKER_ADDRESS = "broker_address";

Config Config::FromJson(const std::string& path) {
    std::ifstream f(path);
    if (!f) {
        throw std::invalid_argument("Unable to open file: " + path);
    }

    nlohmann::json data = nlohmann::json::parse(f);

    return Config{
        .grpc_address = data.at(GRPC_ADDRESS),
        .broker_address = data.at(BROKER_ADDRESS),
    };
}

}  // namespace manager
