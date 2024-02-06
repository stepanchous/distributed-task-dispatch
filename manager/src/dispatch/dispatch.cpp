#include <spdlog/spdlog.h>
#include <unistd.h>

#include <cctype>
#include <condition_variable>
#include <mutex>
#include <unordered_set>
#include <utility>

#include "dispatch.h"
#include "log_format/log_format.h"
#include "task.pb.h"

namespace env {

const char* DB_ADDRESS = "DB_ADDRESS";

}

Dispatcher::ProblemId Dispatcher::PROBLEM_ID = 0;

Dispatcher::Dispatcher(BrokerConnection broker_connection)
    : broker_connection_(std::move(broker_connection)),
      db_client_(grpc::CreateChannel(std::getenv(env::DB_ADDRESS),
                                     grpc::InsecureChannelCredentials())) {}

void Dispatcher::RunDispatcher() {
    while (true) {
        SendComputableTasks();
        UpdateComputableTasks();
    }
}

void Dispatcher::SendComputableTasks() {
    std::lock_guard l(m_);
    for (auto& [problem_id, calc_data] : problem_id_to_calculation_data_) {
        for (const auto& [task_id, dependencies] :
             calc_data.computable_task_to_dependencies) {
            broker_connection_.SendRequest(
                FormTaskRequest(problem_id, task_id, dependencies));
            calc_data.processed_tasks_to_status[task_id] =
                TaskStatus::processing;
        }

        calc_data.computable_task_to_dependencies.clear();
    }
}

void Dispatcher::UpdateComputableTasks() {
    std::optional<task::TaskId> task_id = broker_connection_.ReadReply();

    if (!task_id.has_value()) {
        return;
    }

    {
        std::lock_guard l(m_);

        if (task_id->task_id() ==
            problem_id_to_calculation_data_.at(task_id->problem_id())
                .decompositor.GetSortedTasks()
                .back()) {
            NotifyComputedProblem(task_id->problem_id(), task_id->task_id());
        } else {
            UpdateComputableTasksImpl(task_id->problem_id(),
                                      task_id->task_id());
        }
    }
}

domain::ExprResult Dispatcher::CalculateProblem(dcmp::AST ast) {
    auto calc_data = CalculationData::New(std::move(ast));

    SyncData sync_data;

    ProblemId problem_id = PushProblem(calc_data, sync_data);
    TaskId task_id = calc_data.decompositor.GetSortedTasks().back();

    spdlog::info("Manager got problem with id {}", problem_id);

    {
        std::unique_lock cv_ul(problem_id_to_sync_data_.at(problem_id).cv_m);
        problem_id_to_sync_data_.at(problem_id).cv.wait(cv_ul, [&] {
            return problem_id_to_sync_data_.at(problem_id).is_computed;
        });
    }

    ClearProblem(problem_id);

    domain::ExprResult expr_result =
        db_client_.ReadData(DatabaseClient::DynRecordId{
            .problem_id = problem_id,
            .task_id = task_id,
        });

    db_client_.ClearIntermediateCalculations(problem_id);

    return expr_result;
}

task::Task Dispatcher::FormTaskRequest(
    ProblemId problem_id, TaskId task_id,
    const std::vector<TaskId>& dependencies) const {
    auto id = new task::TaskId;
    id->set_problem_id(problem_id);
    id->set_task_id(task_id);

    task::Task task;
    task.set_allocated_id(id);
    task.set_operation_type(problem_id_to_calculation_data_.at(problem_id)
                                .decompositor.GetOperationType(task_id));

    for (const auto& dependency : dependencies) {
        task::Operand* operand = task.add_operands();
        *operand = FormOperand(problem_id, dependency);
    }

    spdlog::info("Manager -> Broker: {}", task);

    return task;
}

task::Operand Dispatcher::FormOperand(ProblemId problem_id,
                                      TaskId operand_id) const {
    task::Operand operand;

    const CalculationData& calc_data =
        problem_id_to_calculation_data_.at(problem_id);

    if (calc_data.task_to_variable_id.contains(operand_id)) {
        auto static_operand = new task::StaticOperand;
        static_operand->set_identifier(
            calc_data.task_to_variable_id.at(operand_id));

        operand.set_allocated_static_operand(static_operand);
    } else {
        auto task_id = new task::TaskId;
        task_id->set_problem_id(problem_id);
        task_id->set_task_id(operand_id);

        task::DynOperand* dyn_operand = new task::DynOperand;
        dyn_operand->set_allocated_id(task_id);

        operand.set_allocated_dyn_operand(dyn_operand);
    }

    return operand;
}

bool Dispatcher::IsScalarVariable(const std::string& variable_id) {
    return std::all_of(variable_id.begin(), variable_id.end(),
                       [](char c) { return std::islower(c); });
}

Dispatcher::CalculationData::CalculationData(
    dcmp::TaskDecompositor task_decompositor)
    : decompositor(std::move(task_decompositor)) {}

Dispatcher::CalculationData Dispatcher::CalculationData::New(dcmp::AST ast) {
    CalculationData calc_data(dcmp::TaskDecompositor::New(std::move(ast)));

    calc_data.task_to_variable_id =
        calc_data.decompositor.GetIndependentTasks();

    std::unordered_set<TaskId> computed_tasks;

    for (const auto& [task_id, _] : calc_data.task_to_variable_id) {
        calc_data.processed_tasks_to_status.emplace(task_id,
                                                    TaskStatus::completed);
        computed_tasks.emplace(task_id);
    }

    calc_data.computable_task_to_dependencies =
        calc_data.decompositor.GetComputableTasks(computed_tasks);

    return calc_data;
}

void Dispatcher::NotifyComputedProblem(ProblemId problem_id, TaskId task_id) {
    problem_id_to_calculation_data_.at(problem_id)
        .processed_tasks_to_status.at(task_id) = TaskStatus::completed;

    SyncData& sync_data = problem_id_to_sync_data_.at(problem_id);

    {
        std::lock_guard cv_l(sync_data.cv_m);
        sync_data.is_computed = true;
    }

    sync_data.cv.notify_all();
}

Dispatcher::ProblemId Dispatcher::PushProblem(CalculationData calc_data,
                                              SyncData& sync_data) {
    std::lock_guard<std::mutex> l(m_);

    ProblemId problem_id = PROBLEM_ID;

    problem_id_to_calculation_data_.emplace(problem_id, std::move(calc_data));
    problem_id_to_sync_data_.emplace(problem_id, sync_data);

    ++PROBLEM_ID;

    return problem_id;
}

void Dispatcher::ClearProblem(Dispatcher::ProblemId problem_id) {
    std::lock_guard l(m_);

    problem_id_to_calculation_data_.erase(problem_id);
    problem_id_to_sync_data_.erase(problem_id);
}

void Dispatcher::UpdateComputableTasksImpl(ProblemId problem_id,
                                           TaskId task_id) {
    CalculationData& calc_data = problem_id_to_calculation_data_.at(problem_id);

    calc_data.processed_tasks_to_status.at(task_id) = TaskStatus::completed;

    std::unordered_set<TaskId> computed_tasks;
    std::unordered_set<TaskId> excluded_tasks;

    for (const auto& [task_id, status] : calc_data.processed_tasks_to_status) {
        switch (status) {
            case TaskStatus::processing:
                excluded_tasks.emplace(task_id);
                break;
            case TaskStatus::completed:
                computed_tasks.emplace(task_id);
                break;
        }
    }

    calc_data.computable_task_to_dependencies =
        calc_data.decompositor.GetComputableTasks(computed_tasks,
                                                  excluded_tasks);
}
