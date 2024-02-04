#include <fstream>
#include <nlohmann/json.hpp>
#include <stdexcept>

#include "broker_config.h"

namespace broker {

constexpr std::string MANAGER_ADDRESS = "manager_address";
constexpr std::string ROUTER_ADDRESS = "router_address";
constexpr std::string PUB_ADDRESS = "pub_address";

Config Config::FromJson(const std::string& path) {
    std::ifstream f(path);
    if (!f) {
        throw std::invalid_argument("Unable to open file: " + path);
    }

    nlohmann::json data = nlohmann::json::parse(f);

    return Config{
        .manager_address = data.at(MANAGER_ADDRESS),
        .router_address = data.at(ROUTER_ADDRESS),
        .pub_address = data.at(PUB_ADDRESS),
    };
}

}  // namespace broker
