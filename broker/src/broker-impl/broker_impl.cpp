#include <spdlog/spdlog.h>

#include <iostream>

#include "broker_impl.h"
#include "cppzmq/zmq.hpp"
#include "log_format/log_format.h"
#include "task.pb.h"

using namespace std::string_literals;

Broker Broker::New() {
    zmq::context_t context(1);

    std::vector<zmq::socket_t> sockets;

    zmq::socket_t manager_connection(context, zmq::socket_type::pair);
    manager_connection.bind("tcp://*:5559");

    zmq::socket_t worker_pub(context, zmq::socket_type::pub);
    worker_pub.bind("tcp://*:5560");

    zmq::socket_t worker_router(context, zmq::socket_type::router);
    worker_router.bind("tcp://*:5561");

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

void Broker::RunBroker() {
    while (true) {
        if (worker_routing_id_to_status_.empty()) {
            zmq::poll(&poll_items_[0], 1);
        } else {
            zmq::poll(&poll_items_[0], 2);
        }

        if (poll_items_[0].revents & ZMQ_POLLIN) {
            zmq::message_t identity;
            zmq::message_t worker_message;

            worker_router_.recv(identity);
            worker_router_.recv(worker_message);

            std::cout << "Broker got from worker(" << identity.str()
                      << "): " << worker_message.str() << '\n';

            worker_routing_id_to_status_.emplace(std::move(identity), true);
        }

        if (poll_items_[1].revents & ZMQ_POLLIN) {
            zmq::message_t task;
            manager_connection_.recv(task);

            std::cout << task.str() << std::endl;

            task::Task proto_task;
            proto_task.ParseFromArray(task.data(), task.size());

            spdlog::info("Broker <- Manager {}", proto_task);

            worker_pub_.send(zmq::str_buffer("WORK"), zmq::send_flags::sndmore);
            auto hui = worker_pub_.send(task, zmq::send_flags::dontwait);

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

    std::cout << "Broker got from worker(" << identity.str()
              << "): " << worker_message.str() << '\n';

    worker_routing_id_to_status_.emplace(std::move(identity), true);
}
