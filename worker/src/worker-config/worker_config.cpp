#include <fstream>
#include <nlohmann/json.hpp>
#include <stdexcept>

#include "worker_config.h"

namespace worker {

constexpr std::string SUB_ADDRESS = "sub_address";
constexpr std::string DEALER_ADDRESS = "dealer_address";

Config Config::FromJson(const std::string& path) {
    std::ifstream f(path);
    if (!f) {
        throw std::invalid_argument("Unable to open file: " + path);
    }

    nlohmann::json data = nlohmann::json::parse(f);

    return Config{
        .sub_address = data.at(SUB_ADDRESS),
        .dealer_address = data.at(DEALER_ADDRESS),
    };
}

}  // namespace worker
