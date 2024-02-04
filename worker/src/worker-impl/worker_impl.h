#pragma once

#include <cppzmq/zmq.hpp>

#include "worker-impl/thread_pool.h"

class Worker {
   public:
    static Worker New();

    void Run();

   private:
    zmq::context_t context_;
    zmq::socket_t subscriber_;
    zmq::socket_t dealer_;
    std::vector<zmq::pollitem_t> poll_items_;
    std::string identity_;
    size_t core_count_;

    ThreadPool thread_pool_;
    std::queue<task::WorkerTaskId> finished_tasks_{};
    std::mutex tasks_m_{};

    Worker(zmq::context_t context, zmq::socket_t subscriber,
           zmq::socket_t dealer_, std::string identity);

    void SendAckMessage();

    void ReadRequest();

    void CheckOutputQueue();
};
