#pragma once

#include <condition_variable>
#include <mutex>
#include <queue>
#include <thread>
#include <vector>

#include "database-client/database_client.h"
#include "worker.pb.h"

class ThreadPool {
   public:
    ThreadPool(size_t thread_count,
               std::queue<task::WorkerTaskId>& output_queue,
               std::mutex& output_m);

    void PushTask(task::WorkerTask task);

   private:
    std::vector<std::thread> threads_;
    std::queue<task::WorkerTask> tasks_;
    std::mutex tasks_m_;
    std::condition_variable condition;

    std::queue<task::WorkerTaskId>& output_queue_;
    std::mutex& output_m_;

    DatabaseClient db_client_;

    void Poll();

    void CalculateTask(const task::WorkerTask& task);

    static domain::ExprResult CalculateTaskImpl(
        domain::ExprType type, std::vector<domain::ExprResult> operands);

    std::vector<domain::ExprResult> ExtractOperands(
        const task::WorkerTask& task);

    static std::pair<domain::ExprResult, std::optional<domain::ExprResult>>
    PrepareOperands(const std::vector<domain::ExprResult>& operands);
};
