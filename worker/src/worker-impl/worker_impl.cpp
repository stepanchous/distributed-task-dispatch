#include <cstdlib>
#include <string>

#include "cppzmq/zmq.hpp"
#include "worker.pb.h"
#include "worker_impl.h"

const char* CORE_COUNT = "CORE_COUNT";
const char* WORKER_IDENTITY = "WORKER_IDENTITY";

Worker Worker::New(const worker::Config& config) {
    zmq::context_t context(1);

    zmq::socket_t subscriber(context, zmq::socket_type::sub);
    subscriber.set(zmq::sockopt::subscribe, "");
    subscriber.connect(config.sub_address);

    zmq::socket_t dealer(context, zmq::socket_type::dealer);
    dealer.set(zmq::sockopt::routing_id, std::getenv(WORKER_IDENTITY));
    dealer.connect(config.dealer_address);

    return Worker(std::move(context), std::move(subscriber), std::move(dealer));
}

Worker::Worker(zmq::context_t context, zmq::socket_t subscriber,
               zmq::socket_t dealer)
    : context_(std::move(context)),
      subscriber_(std::move(subscriber)),
      dealer_(std::move(dealer)),
      core_count_(std::stoi(std::getenv(CORE_COUNT))) {
    poll_items_.push_back({subscriber_, 0, ZMQ_POLLIN, 0});
}

void Worker::Run() {
    SendAckMessage();

    while (true) {
        zmq::poll(&poll_items_[0], 1);

        if (poll_items_[0].revents & ZMQ_POLLIN) {
            zmq::message_t topic;
            zmq::message_t request;

            subscriber_.recv(topic, zmq::recv_flags::dontwait);
            subscriber_.recv(request, zmq::recv_flags::dontwait);
        }
    }
}

void Worker::SendAckMessage() {
    worker::Message ack_message;

    ack_message.set_core_count(core_count_);

    dealer_.send(zmq::message_t(ack_message.SerializeAsString()),
                 zmq::send_flags::none);
}
