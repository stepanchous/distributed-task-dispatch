#pragma once

#include <cppzmq/zmq.hpp>

template <typename Protobuf>
Protobuf FromMessage(const zmq::message_t& message) {
    Protobuf protobuf;

    if (!protobuf.ParseFromArray(message.data(), message.size())) {
        throw std::domain_error("Unable to parse message");
    }

    return protobuf;
}
