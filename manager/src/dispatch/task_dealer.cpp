#include <iostream>

#include "cppzmq/zmq.hpp"
#include "task_dealer.h"

TaskDealer TaskDealer::New() {
    zmq::context_t context(1);

    zmq::socket_t broker_connection(context, zmq::socket_type::pair);
    broker_connection.set(zmq::sockopt::routing_id, "manager");
    broker_connection.connect("tcp://localhost:5559");

    return TaskDealer(std::move(context), std::move(broker_connection));
}

TaskDealer::TaskDealer(zmq::context_t context, zmq::socket_t dealer)
    : context_(std::move(context)), broker_connection_(std::move(dealer)) {}

void TaskDealer::SendRequest(const std::string& request) {
    auto res =
        broker_connection_.send(zmq::message_t(request), zmq::send_flags::none);

    if (res) {
        std::cout << "request sent" << std::endl;
    } else {
        std::cout << "could not send request" << std::endl;
    }
}

std::optional<std::string> TaskDealer::ReadReply() {
    zmq::message_t reply;
    std::optional<size_t> byte_count =
        broker_connection_.recv(reply, zmq::recv_flags::dontwait);

    if (!byte_count.has_value()) {
        return {};
    }

    std::cout << "Recieved from broker: " << reply.str() << '\n';

    return reply.str();
}
