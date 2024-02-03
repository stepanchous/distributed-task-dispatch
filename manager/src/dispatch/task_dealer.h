#pragma once

#include <cppzmq/zmq.hpp>

class BrokerConnection {
   public:
    static BrokerConnection New(/* config */);

    void SendRequest(const std::string& request);

    std::optional<std::string> ReadReply();

   private:
    BrokerConnection(zmq::context_t context, zmq::socket_t dealer);

    zmq::context_t context_;
    zmq::socket_t broker_connection_;
};
