#include <spdlog/spdlog.h>

#include <cstdlib>
#include <stdexcept>

#include "broker_impl.h"
#include "communiation/read_message.h"
#include "cppzmq/zmq.hpp"
#include "log_format/log_format.h"
#include "task.pb.h"
#include "worker.pb.h"

using namespace std::string_literals;

namespace env {

const char* ZMQ_MANAGER_ADDRESS = "ZMQ_MANAGER_ADDRESS";
const char* ZMQ_PUB_ADDRESS = "ZMQ_PUB_ADDRESS";
const char* ZMQ_ROUTER_ADDRESS = "ZMQ_ROUTER_ADDRESS";

}  // namespace env

const std::string Broker::TOPIC = "work";

Broker Broker::New() {
    zmq::context_t context(1);

    std::vector<zmq::socket_t> sockets;

    std::string manager_address = std::getenv(env::ZMQ_MANAGER_ADDRESS);
    std::string pub_address = std::getenv(env::ZMQ_PUB_ADDRESS);
    std::string router_address = std::getenv(env::ZMQ_ROUTER_ADDRESS);

    zmq::socket_t manager_connection(context, zmq::socket_type::pair);
    manager_connection.bind(manager_address);

    spdlog::info("Broker is listening manager on {}", manager_address);

    zmq::socket_t worker_pub(context, zmq::socket_type::pub);
    worker_pub.bind(pub_address);

    spdlog::info("Broker is publishing to {}", pub_address);

    zmq::socket_t worker_router(context, zmq::socket_type::router);
    worker_router.bind(router_address);

    spdlog::info("Broker is listening workers on {}", router_address);

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
    spdlog::info("Broker started");

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
            ReceiveManagerRequest();
        }
    }
}

void Broker::ReceiveWorkerMessage() {
    zmq::message_t identity;
    zmq::message_t worker_message;

    worker_router_.recv(identity);
    worker_router_.recv(worker_message);

    auto message = FromMessage<task::WorkerMessage>(worker_message);

    std::string str_identity = std::string(
        static_cast<const char*>(identity.data()),
        static_cast<const char*>(identity.data()) + identity.size());

    if (message.has_core_count()) {
        RegisterWorker(str_identity, message);
    } else if (message.has_task_id()) {
        ProcessWorkerTaskReply(message);
    } else {
        throw std::logic_error("Invalid message");
    }
}

void Broker::RegisterWorker(const std::string& identity,
                            const task::WorkerMessage& message) {
    auto [_, ok] = worker_routing_id_to_available_core_count_.try_emplace(
        identity, message.core_count());

    if (!ok) {
        throw std::logic_error(
            "Worker with exisiting identity tried to register");
    }

    spdlog::info("Register Worker({}) {}", identity, message.core_count());
}

void Broker::ProcessWorkerTaskReply(const task::WorkerMessage& message) {
    spdlog::info("Broker <- Worker {}", message);

    const std::string& identity = message.task_id().identity();

    ++worker_routing_id_to_available_core_count_.at(identity);

    if (!task_queue_.empty()) {
        SendTaskToWorkerFromQueue(identity);
    }

    SendResultToManager(message);
}

void Broker::SendTaskToWorkerFromQueue(const std::string& identity) {
    task::Task manager_task = task_queue_.back();
    task_queue_.pop();

    task::WorkerTaskId worker_task;
    worker_task.set_identity(identity);
    worker_task.set_allocated_id(new task::TaskId(manager_task.id()));

    SendTaskToWorkerImpl(worker_task);
}

void Broker::SendResultToManager(const task::WorkerMessage& worker_message) {
    manager_connection_.send(
        zmq::message_t(worker_message.task_id().id().SerializeAsString()),
        zmq::send_flags::none);

    spdlog::info("Broker -> Manager {}", worker_message.task_id());
}

void Broker::SendTaskToWorker(const task::Task& manager_task) {
    auto it = std::find_if(worker_routing_id_to_available_core_count_.begin(),
                           worker_routing_id_to_available_core_count_.end(),
                           [](const auto& worker_to_core_count) {
                               return worker_to_core_count.second != 0;
                           });

    if (it == worker_routing_id_to_available_core_count_.end()) {
        task_queue_.push(manager_task);

        spdlog::info("Task was added to queue {}", manager_task);
    } else {
        task::WorkerTaskId worker_task;
        worker_task.set_identity(it->first);
        worker_task.set_allocated_id(new task::TaskId(manager_task.id()));

        SendTaskToWorkerImpl(worker_task);
    }
}

void Broker::SendTaskToWorkerImpl(const task::WorkerTaskId& worker_task) {
    worker_pub_.send(zmq::message_t(TOPIC), zmq::send_flags::sndmore);
    worker_pub_.send(zmq::message_t(worker_task.SerializeAsString()),
                     zmq::send_flags::none);

    --worker_routing_id_to_available_core_count_.at(worker_task.identity());

    spdlog::info("Broker -> Worker {}", worker_task);
}

void Broker::ReceiveManagerRequest() {
    zmq::message_t manager_message;
    manager_connection_.recv(manager_message);

    auto manager_task = FromMessage<task::Task>(manager_message);

    spdlog::info("Broker <- Manager {}", manager_task);

    SendTaskToWorker(manager_task);
}
