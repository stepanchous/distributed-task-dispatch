#pragma once

#include <string>

namespace manager {

struct Config {
    static Config FromJson(const std::string path);

    std::string addr_uri;
};

}  // namespace manager
