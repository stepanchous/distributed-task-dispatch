#pragma once

#include <string>

namespace broker {

struct Config {
    static Config FromJson(const std::string& path);

    std::string manager_address;
    std::string router_address;
    std::string pub_address;
};

}  // namespace broker
