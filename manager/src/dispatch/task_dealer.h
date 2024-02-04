#pragma once

#include <cppzmq/zmq.hpp>

#include "manager-config/manager_config.h"

class BrokerConnection {
   public:
    static BrokerConnection New(const manager::Config& config);

    void SendRequest(const std::string& request);

    std::optional<std::string> ReadReply();

   private:
    BrokerConnection(zmq::context_t context, zmq::socket_t dealer);

    zmq::context_t context_;
    zmq::socket_t broker_connection_;
};
