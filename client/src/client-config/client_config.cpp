#include <fstream>
#include <nlohmann/json.hpp>
#include <stdexcept>

#include "client_config.h"

namespace client {

constexpr std::string TARGET = "target";

Config Config::FromJson(const std::string path) {
    std::ifstream f(path);
    if (!f) {
        throw std::invalid_argument("Unable to open file: " + path);
    }

    nlohmann::json data = nlohmann::json::parse(f);

    return Config{
        .target = data.at(TARGET),
    };
}

}  // namespace client
