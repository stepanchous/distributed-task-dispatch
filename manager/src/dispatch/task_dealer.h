#pragma once

#include <cppzmq/zmq.hpp>

#include "task.pb.h"

class BrokerConnection {
   public:
    static BrokerConnection New();

    void SendRequest(task::Task task);

    std::optional<task::TaskId> ReadReply();

   private:
    BrokerConnection(zmq::context_t context, zmq::socket_t dealer);

    zmq::context_t context_;
    zmq::socket_t broker_connection_;
};
