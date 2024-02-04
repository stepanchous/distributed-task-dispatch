#include <spdlog/spdlog.h>

#include <iostream>
#include <stdexcept>

#include "broker_impl.h"
#include "communiation/read_message.h"
#include "cppzmq/zmq.hpp"
#include "log_format/log_format.h"
#include "task.pb.h"
#include "worker.pb.h"

using namespace std::string_literals;

Broker Broker::New(const broker::Config& config) {
    zmq::context_t context(1);

    std::vector<zmq::socket_t> sockets;

    zmq::socket_t manager_connection(context, zmq::socket_type::pair);
    manager_connection.bind(config.manager_address);

    zmq::socket_t worker_pub(context, zmq::socket_type::pub);
    worker_pub.bind(config.pub_address);

    zmq::socket_t worker_router(context, zmq::socket_type::router);
    worker_router.bind(config.router_address);

    return Broker(std::move(context), std::move(manager_connection),
                  std::move(worker_pub), std::move(worker_router));
}

Broker::Broker(zmq::context_t context, zmq::socket_t manager_connetion,
               zmq::socket_t worker_pub, zmq::socket_t worker_router)
    : context_(std::move(context)),
      manager_connection_(std::move(manager_connetion)),
      worker_pub_(std::move(worker_pub)),
      worker_router_(std::move(worker_router)) {
    poll_items_.push_back({worker_router_, 0, ZMQ_POLLIN, 0});
    poll_items_.push_back({manager_connection_, 0, ZMQ_POLLIN, 0});
}

void Broker::Run() {
    while (true) {
        if (worker_routing_id_to_available_core_count_.empty()) {
            zmq::poll(&poll_items_[0], 1);
        } else {
            zmq::poll(&poll_items_[0], 2);
        }

        if (poll_items_[0].revents & ZMQ_POLLIN) {
            ReceiveWorkerMessage();
        }

        if (poll_items_[1].revents & ZMQ_POLLIN) {
            zmq::message_t task;
            manager_connection_.recv(task);

            auto proto_task = FromMessage<task::Task>(task);

            spdlog::info("Broker <- Manager {}", proto_task);

            worker_pub_.send(zmq::str_buffer("WORK"), zmq::send_flags::sndmore);
            auto hui = worker_pub_.send(task, zmq::send_flags::dontwait);

            spdlog::info("Broker -> Worker {}", proto_task);

            if (hui.has_value()) {
                std::cout << "Message was sent to worker" << std::endl;
            } else {
                std::cout << "Unable to send message to worker" << std::endl;
            }
        }
    }
}

void Broker::ReceiveWorkerMessage() {
    zmq::message_t identity;
    zmq::message_t worker_message;

    worker_router_.recv(identity);
    worker_router_.recv(worker_message);

    auto message = FromMessage<worker::Message>(worker_message);

    std::string str_identity = std::string(
        static_cast<const char*>(identity.data()),
        static_cast<const char*>(identity.data()) + identity.size());

    if (message.has_core_count()) {
        auto [_, ok] = worker_routing_id_to_available_core_count_.try_emplace(
            str_identity, message.core_count());

        if (!ok) {
            throw std::logic_error(
                "Worker with exisiting identity tried to register");
        }
    } else if (message.has_task_id()) {
        worker_routing_id_to_available_core_count_.at(str_identity)++;
        manager_connection_.send(worker_message, zmq::send_flags::none);
    } else {
        throw std::logic_error("Invalid message");
    }

    spdlog::info("Broker <- Worker {}", message);
}
