#pragma once

#include <condition_variable>
#include <mutex>
#include <queue>
#include <thread>
#include <vector>

#include "worker.pb.h"

class ThreadPool {
   public:
    ThreadPool(size_t thread_count,
               std::queue<task::WorkerTaskId>& output_queue,
               std::mutex& output_m);

    void PushTask(task::WorkerTaskId task);

   private:
    std::vector<std::thread> threads_;
    std::queue<task::WorkerTaskId> tasks_;
    std::mutex tasks_m_;
    std::condition_variable condition;

    std::queue<task::WorkerTaskId>& output_queue_;
    std::mutex& output_m_;

    void Poll();

    void CalculateTask(const task::WorkerTaskId& task);
};
