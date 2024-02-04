#pragma once

#include <cppzmq/zmq.hpp>
#include <queue>
#include <unordered_map>

#include "task.pb.h"
#include "worker.pb.h"

class Broker {
   public:
    static Broker New();

    void Run();

   private:
    Broker(zmq::context_t context, zmq::socket_t manager_connetion,
           zmq::socket_t worker_pub, zmq::socket_t worker_router);

    void LogManagerTask(const std::string& str_task) const;

   private:
    void ReceiveWorkerMessage();

    void RegisterWorker(const std::string& identity,
                        const task::WorkerMessage& message);

    void ProcessWorkerTaskReply(const task::WorkerMessage& message);

    void SendTaskToWorkerFromQueue(const std::string& identity);

    void SendResultToManager(const task::WorkerMessage& worker_message);

    void SendTaskToWorker(const task::Task& manager_task);

    void SendTaskToWorkerImpl(const task::WorkerTaskId& worker_task);

    void ReceiveManagerRequest();

   private:
    zmq::context_t context_;
    zmq::socket_t manager_connection_;
    zmq::socket_t worker_pub_;
    zmq::socket_t worker_router_;
    std::vector<zmq::pollitem_t> poll_items_;
    std::unordered_map<std::string, size_t>
        worker_routing_id_to_available_core_count_{};
    std::queue<task::Task> task_queue_{};

    static const std::string TOPIC;
};
