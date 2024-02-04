#include "thread_pool.h"

ThreadPool::ThreadPool(size_t thread_count,
                       std::queue<task::WorkerTaskId>& output_queue,
                       std::mutex& output_m)
    : output_queue_(output_queue), output_m_(output_m) {
    for (size_t i = 0; i < thread_count; ++i) {
        threads_.emplace_back([this] { Poll(); });
    }
}

void ThreadPool::PushTask(task::WorkerTaskId task) {
    {
        std::unique_lock l(tasks_m_);
        tasks_.push(std::move(task));
    }

    condition.notify_one();
}

void ThreadPool::Poll() {
    while (true) {
        task::WorkerTaskId task;

        {
            std::unique_lock l(tasks_m_);

            condition.wait(l, [this] { return !tasks_.empty(); });

            task = std::move(tasks_.front());
            tasks_.pop();
        }

        CalculateTask(task);
    }
}

void ThreadPool::CalculateTask(const task::WorkerTaskId& task) {
    // TODO: impelemnt logic
    sleep(10);

    {
        std::lock_guard l(output_m_);
        output_queue_.push(task);
    }
}
