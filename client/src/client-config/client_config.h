#pragma once

#include <string>

namespace client {

struct Config {
    static Config FromJson(const std::string path);

    std::string target;
};

}  // namespace client
