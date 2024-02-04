#pragma once

#include <string>

namespace worker {

struct Config {
    static Config FromJson(const std::string& path);

    std::string sub_address;
    std::string dealer_address;
};

}  // namespace worker
