#include <spdlog/spdlog.h>

#include "communiation/read_message.h"
#include "cppzmq/zmq.hpp"
#include "log_format/log_format.h"
#include "task_dealer.h"

namespace env {

const char* ZMQ_BROKER_ADDRESS = "ZMQ_BROKER_ADDRESS";

}

BrokerConnection BrokerConnection::New() {
    zmq::context_t context(1);

    zmq::socket_t broker_connection(context, zmq::socket_type::pair);
    broker_connection.connect(std::getenv(env::ZMQ_BROKER_ADDRESS));

    return BrokerConnection(std::move(context), std::move(broker_connection));
}

BrokerConnection::BrokerConnection(zmq::context_t context, zmq::socket_t dealer)
    : context_(std::move(context)), broker_connection_(std::move(dealer)) {}

void BrokerConnection::SendRequest(task::Task task) {
    auto res = broker_connection_.send(zmq::message_t(task.SerializeAsString()),
                                       zmq::send_flags::none);

    if (res) {
        spdlog::info("Message sent");
    } else {
        spdlog::error("Message was not sent");
    }
}

std::optional<task::TaskId> BrokerConnection::ReadReply() {
    zmq::message_t reply;
    std::optional<size_t> byte_count =
        broker_connection_.recv(reply, zmq::recv_flags::dontwait);

    if (!byte_count.has_value()) {
        return {};
    }

    auto finished_task = FromMessage<task::TaskId>(reply);

    spdlog::info("Manager <- Broker {}", finished_task);

    return finished_task;
}
