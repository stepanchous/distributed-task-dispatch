#include <grpcpp/grpcpp.h>
#include <spdlog/spdlog.h>

#include <string>
#include <variant>

#include "log_format/log_format.h"
#include "thread_pool.h"

namespace env {

const char* DB_ADDRESS = "DB_ADDRESS";

}

ThreadPool::ThreadPool(size_t thread_count,
                       std::queue<task::WorkerTaskId>& output_queue,
                       std::mutex& output_m)
    : output_queue_(output_queue),
      output_m_(output_m),
      db_client_(grpc::CreateChannel(std::getenv(env::DB_ADDRESS),
                                     grpc::InsecureChannelCredentials())) {
    for (size_t i = 0; i < thread_count; ++i) {
        threads_.emplace_back([this] { Poll(); });
    }
}

void ThreadPool::PushTask(task::WorkerTask task) {
    {
        std::unique_lock l(tasks_m_);
        tasks_.push(std::move(task));
    }

    condition.notify_one();
}

void ThreadPool::Poll() {
    while (true) {
        task::WorkerTask task;

        {
            std::unique_lock l(tasks_m_);

            condition.wait(l, [this] { return !tasks_.empty(); });

            task = std::move(tasks_.front());
            tasks_.pop();
        }

        CalculateTask(task);
    }
}

void ThreadPool::CalculateTask(const task::WorkerTask& task) {
    domain::ExprResult expr_result = CalculateTaskImpl(
        static_cast<domain::ExprType>(task.task().operation_type()),
        ExtractOperands(task));

    db_client_.WriteDynamicData(task.task().id().problem_id(),
                                task.task().id().task_id(), expr_result);

    task::WorkerTaskId response;
    response.set_identity(task.identity());
    response.set_allocated_id(new task::TaskId(task.task().id()));

    spdlog::info("[Calculated result] task: {}, result: {}", response,
                 expr_result);

    {
        std::lock_guard l(output_m_);
        output_queue_.push(response);
    }
}

std::vector<domain::ExprResult> ThreadPool::ExtractOperands(
    const task::WorkerTask& task) {
    std::vector<domain::ExprResult> operands;

    for (const auto& operand : task.task().operands()) {
        if (operand.has_static_operand()) {
            operands.push_back(
                db_client_.ReadData(operand.static_operand().identifier()));
        } else if (operand.has_dyn_operand()) {
            operands.push_back(db_client_.ReadData(DatabaseClient::DynRecordId{
                .problem_id = operand.dyn_operand().id().problem_id(),
                .task_id = operand.dyn_operand().id().task_id(),
            }));
        } else {
            throw std::logic_error(
                "Invalid message file:" + std::string(__FILE__) +
                "; line:" + std::to_string(__LINE__));
        }
    }

    return operands;
}

domain::ExprResult ThreadPool::CalculateTaskImpl(
    domain::ExprType type, std::vector<domain::ExprResult> operands) {
    domain::ExprResult result;

    auto destructured_operands = PrepareOperands(operands);

    domain::ExprResult lhs = destructured_operands.first;
    domain::ExprResult rhs = destructured_operands.second.value_or(0);

    switch (type) {
        case domain::ExprType::re_min:
            result = domain::Min(std::get<domain::List>(lhs));
            break;
        case domain::ExprType::re_max:
            result = domain::Max(std::get<domain::List>(lhs));
            break;
        case domain::ExprType::re_mul:
            result = domain::Mul(std::get<domain::List>(lhs));
            break;
        case domain::ExprType::re_sum:
            result = domain::Sum(std::get<domain::List>(lhs));
            break;
        case domain::ExprType::re_size:
            result = domain::Size(std::get<domain::List>(lhs));
            break;
        case domain::ExprType::me_add:
            result = domain::Sum(std::get<domain::List>(lhs),
                                 std::get<domain::Scalar>(rhs));
            break;
        case domain::ExprType::me_mul:
            result = domain::Mul(std::get<domain::List>(lhs),
                                 std::get<domain::Scalar>(rhs));
            break;
        case domain::ExprType::me_div:
            result = domain::Div(std::get<domain::List>(lhs),
                                 std::get<domain::Scalar>(rhs));
            break;
        case domain::ExprType::lo_add:
            result = domain::Sum(std::get<domain::List>(lhs),
                                 std::get<domain::List>(rhs));
            break;
        case domain::ExprType::lo_mul:
            result = domain::Mul(std::get<domain::List>(lhs),
                                 std::get<domain::List>(rhs));
            break;
        case domain::ExprType::lo_div:
            result = domain::Div(std::get<domain::List>(lhs),
                                 std::get<domain::List>(rhs));
            break;
        case domain::ExprType::lo_dot:
            result = domain::Dot(std::get<domain::List>(lhs),
                                 std::get<domain::List>(rhs));
            break;
        case domain::ExprType::so_add:
            result = domain::Sum(std::get<domain::Scalar>(lhs),
                                 std::get<domain::Scalar>(rhs));
            break;
        case domain::ExprType::so_mul:
            result = domain::Mul(std::get<domain::Scalar>(lhs),
                                 std::get<domain::Scalar>(rhs));
            break;
        case domain::ExprType::so_div:
            result = domain::Div(std::get<domain::Scalar>(lhs),
                                 std::get<domain::Scalar>(rhs));
            break;
        case domain::ExprType::so_max:
            result = domain::Max(std::get<domain::Scalar>(lhs),
                                 std::get<domain::Scalar>(rhs));
            break;
        case domain::ExprType::so_min:
            result = domain::Min(std::get<domain::Scalar>(lhs),
                                 std::get<domain::Scalar>(rhs));
            break;
        default:
            throw std::logic_error("Invalid operation");
            break;
    }

    return result;
}

std::pair<domain::ExprResult, std::optional<domain::ExprResult>>
ThreadPool::PrepareOperands(const std::vector<domain::ExprResult>& operands) {
    if (operands.size() == 1) {
        return std::make_pair(operands.front(), std::nullopt);
    }

    if (std::holds_alternative<domain::Scalar>(operands.front())) {
        return std::make_pair(operands.back(), operands.front());
    }

    return std::make_pair(operands.back(), operands.front());
}
