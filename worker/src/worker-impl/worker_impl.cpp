#include <spdlog/spdlog.h>

#include <cstdlib>
#include <string>

#include "communiation/read_message.h"
#include "cppzmq/zmq.hpp"
#include "log_format/log_format.h"
#include "worker.pb.h"
#include "worker_impl.h"

Worker Worker::New() {
    zmq::context_t context(1);

    std::string sub_address = std::getenv("ZMQ_SUB_ADDRESS");
    std::string dealer_address = std::getenv("ZMQ_DEALER_ADDRESS");
    std::string identity = std::getenv("WORKER_IDENTITY");

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
      core_count_(std::stoi(std::getenv("CORE_COUNT"))) {
    poll_items_.push_back({subscriber_, 0, ZMQ_POLLIN, 0});
}

void Worker::Run() {
    spdlog::info("Worker started");

    SendAckMessage();

    while (true) {
        zmq::poll(&poll_items_[0], 1);

        if (poll_items_[0].revents & ZMQ_POLLIN) {
            zmq::message_t topic;
            zmq::message_t request;

            subscriber_.recv(topic, zmq::recv_flags::dontwait);

            if (subscriber_.recv(request, zmq::recv_flags::dontwait)) {
                task::WorkerTaskId task =
                    FromMessage<task::WorkerTaskId>(request);

                if (task.identity() != identity_) {
                    spdlog::info("Discarding request");
                    continue;
                }

                spdlog::info("Worker <- Broker {}", task);

                sleep(10);
            }

            spdlog::info("Finished work");
        }
    }
}

void Worker::SendAckMessage() {
    task::WorkerMessage ack_message;

    ack_message.set_core_count(core_count_);

    dealer_.send(zmq::message_t(ack_message.SerializeAsString()),
                 zmq::send_flags::none);
}
