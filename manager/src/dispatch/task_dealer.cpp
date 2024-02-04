#include <spdlog/spdlog.h>

#include <iostream>

#include "cppzmq/zmq.hpp"
#include "task_dealer.h"

BrokerConnection BrokerConnection::New(const manager::Config& config) {
    zmq::context_t context(1);

    zmq::socket_t broker_connection(context, zmq::socket_type::pair);
    broker_connection.connect(config.broker_address);

    return BrokerConnection(std::move(context), std::move(broker_connection));
}

BrokerConnection::BrokerConnection(zmq::context_t context, zmq::socket_t dealer)
    : context_(std::move(context)), broker_connection_(std::move(dealer)) {}

void BrokerConnection::SendRequest(const std::string& request) {
    auto res =
        broker_connection_.send(zmq::message_t(request), zmq::send_flags::none);

    if (res) {
        spdlog::info("Message sent");
    } else {
        spdlog::error("Message was not sent");
    }
}

std::optional<std::string> BrokerConnection::ReadReply() {
    zmq::message_t reply;
    std::optional<size_t> byte_count =
        broker_connection_.recv(reply, zmq::recv_flags::dontwait);

    if (!byte_count.has_value()) {
        return {};
    }

    std::cout << "Recieved from broker: " << reply.str() << '\n';

    return reply.str();
}
