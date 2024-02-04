#pragma once

#include <cppzmq/zmq.hpp>

#include "worker-config/worker_config.h"

class Worker {
   public:
    static Worker New(const worker::Config& config);

    void Run();

   private:
    Worker(zmq::context_t context, zmq::socket_t subscriber,
           zmq::socket_t dealer_);

    void SendAckMessage();

   private:
    zmq::context_t context_;
    zmq::socket_t subscriber_;
    zmq::socket_t dealer_;
    std::vector<zmq::pollitem_t> poll_items_;
    size_t core_count_;
};
