#include <fstream>
#include <nlohmann/json.hpp>
#include <stdexcept>

#include "manager_config.h"

namespace manager {

constexpr std::string ADDR_URI = "addr_uri";

Config Config::FromJson(const std::string path) {
    std::ifstream f(path);
    if (!f) {
        throw std::invalid_argument("Unable to open file: " + path);
    }

    nlohmann::json data = nlohmann::json::parse(f);

    return Config{
        .addr_uri = data.at(ADDR_URI),
    };
}

}  // namespace manager
