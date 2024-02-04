#pragma once

#include <cppzmq/zmq.hpp>

class Worker {
   public:
    static Worker New();

    void Run();

   private:
    Worker(zmq::context_t context, zmq::socket_t subscriber,
           zmq::socket_t dealer_, std::string identity);

    void SendAckMessage();

   private:
    zmq::context_t context_;
    zmq::socket_t subscriber_;
    zmq::socket_t dealer_;
    std::vector<zmq::pollitem_t> poll_items_;
    std::string identity_;
    size_t core_count_;
};
