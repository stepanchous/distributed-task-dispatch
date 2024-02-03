#pragma once

#include <cppzmq/zmq.hpp>
#include <unordered_map>

class Broker {
   public:
    static Broker New(/* config */);

    void RunBroker();

   private:
    Broker(zmq::context_t context, zmq::socket_t manager_connetion,
           zmq::socket_t worker_pub, zmq::socket_t worker_router);

    struct MessageHasher {
        size_t operator()(const zmq::message_t& message) const {
            return std::hash<std::string>{}(message.str());
        }
    };

    void LogManagerTask(const std::string& str_task) const;

   private:
    zmq::context_t context_;
    zmq::socket_t manager_connection_;
    zmq::socket_t worker_pub_;
    zmq::socket_t worker_router_;
    std::vector<zmq::pollitem_t> poll_items_;
    std::unordered_map<zmq::message_t, bool, MessageHasher>
        worker_routing_id_to_status_{};
};
