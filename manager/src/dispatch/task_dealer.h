#pragma once

#include <cppzmq/zmq.hpp>

class TaskDealer {
   public:
    static TaskDealer New(/* config */);

    void SendRequest(const std::string& request);

    std::optional<std::string> ReadReply();

   private:
    TaskDealer(zmq::context_t context, zmq::socket_t dealer);

    zmq::context_t context_;
    zmq::socket_t broker_connection_;
};
