#include <spdlog/spdlog.h>

#include <chrono>
#include <cstdlib>
#include <string>

#include "communiation/read_message.h"
#include "cppzmq/zmq.hpp"
#include "log_format/log_format.h"
#include "worker-impl/thread_pool.h"
#include "worker.pb.h"
#include "worker_impl.h"

namespace env {

const char* ZMQ_SUB_ADDRESS = "ZMQ_SUB_ADDRESS";
const char* ZMQ_DEALER_ADDRESS = "ZMQ_DEALER_ADDRESS";
const char* WORKER_IDENTITY = "WORKER_IDENTITY";
const char* CORE_COUNT = "CORE_COUNT";

}  // namespace env

Worker Worker::New() {
    zmq::context_t context(1);

    std::string sub_address = std::getenv(env::ZMQ_SUB_ADDRESS);
    std::string dealer_address = std::getenv(env::ZMQ_DEALER_ADDRESS);
    std::string identity = std::getenv(env::WORKER_IDENTITY);

    zmq::socket_t subscriber(context, zmq::socket_type::sub);
    subscriber.set(zmq::sockopt::subscribe, "");
    subscriber.connect(sub_address);

    spdlog::info("Worker connected to publisher on {}", sub_address);

    zmq::socket_t dealer(context, zmq::socket_type::dealer);
    dealer.set(zmq::sockopt::routing_id, identity);
    dealer.connect(dealer_address);

    spdlog::info("Worker connected to dealer on {}", dealer_address);

    return Worker(std::move(context), std::move(subscriber), std::move(dealer),
                  std::move(identity));
}

Worker::Worker(zmq::context_t context, zmq::socket_t subscriber,
               zmq::socket_t dealer, std::string identity)
    : context_(std::move(context)),
      subscriber_(std::move(subscriber)),
      dealer_(std::move(dealer)),
      identity_(std::move(identity)),
      core_count_(std::stoi(std::getenv(env::CORE_COUNT))),
      thread_pool_(core_count_, finished_tasks_, tasks_m_) {
    poll_items_.push_back({subscriber_, 0, ZMQ_POLLIN, 0});
}

void Worker::Run() {
    spdlog::info("Worker started");

    SendAckMessage();

    while (true) {
        zmq::poll(&poll_items_[0], 1, std::chrono::milliseconds(0));

        if (poll_items_[0].revents & ZMQ_POLLIN) {
            ReadRequest();
        }

        CheckOutputQueue();
    }
}

void Worker::SendAckMessage() {
    task::WorkerMessage ack_message;

    ack_message.set_core_count(core_count_ - 1);

    dealer_.send(zmq::message_t(ack_message.SerializeAsString()),
                 zmq::send_flags::none);
}

void Worker::ReadRequest() {
    zmq::message_t topic;
    zmq::message_t request;

    subscriber_.recv(topic, zmq::recv_flags::none);
    subscriber_.recv(request, zmq::recv_flags::none);

    auto task = FromMessage<task::WorkerTaskId>(request);

    if (task.identity() != identity_) {
        return;
    }

    spdlog::info("Worker <- Broker {}", task);

    thread_pool_.PushTask(std::move(task));
}

void Worker::CheckOutputQueue() {
    task::WorkerTaskId finished_task;

    {
        std::lock_guard l(tasks_m_);

        if (finished_tasks_.empty()) {
            return;
        }

        finished_task = std::move(finished_tasks_.front());
        finished_tasks_.pop();
    }

    task::WorkerMessage worker_message;
    worker_message.set_allocated_task_id(
        new task::WorkerTaskId(std::move(finished_task)));

    dealer_.send(zmq::message_t(worker_message.SerializeAsString()),
                 zmq::send_flags::none);
}
