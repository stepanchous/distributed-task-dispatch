#pragma once

#include <cppzmq/zmq.hpp>
#include <unordered_map>

#include "broker-config/broker_config.h"

class Broker {
   public:
    static Broker New(const broker::Config& config);

    void Run();

   private:
    Broker(zmq::context_t context, zmq::socket_t manager_connetion,
           zmq::socket_t worker_pub, zmq::socket_t worker_router);

    void LogManagerTask(const std::string& str_task) const;

   private:
    void ReceiveWorkerMessage();

   private:
    zmq::context_t context_;
    zmq::socket_t manager_connection_;
    zmq::socket_t worker_pub_;
    zmq::socket_t worker_router_;
    std::vector<zmq::pollitem_t> poll_items_;
    std::unordered_map<std::string, size_t>
        worker_routing_id_to_available_core_count_{};
};
