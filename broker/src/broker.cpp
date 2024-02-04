#include <iostream>

#include "broker-impl/broker_impl.h"

int main(int argc, char* argv[]) {
    if (argc != 2) {
        std::cerr << "Invalid number of arguments" << std::endl;
    }

    Broker::New(broker::Config::FromJson(argv[1])).Run();
}
