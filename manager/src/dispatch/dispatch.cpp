#include <spdlog/spdlog.h>
#include <unistd.h>

#include <cctype>
#include <condition_variable>
#include <mutex>
#include <utility>
#include <variant>

#include "dispatch.h"
#include "domain/domain.h"
#include "log_format/log_format.h"
#include "task.pb.h"

ProblemId Dispatcher::PROBLEM_ID = 0;

Dispatcher::Dispatcher(BrokerConnection& broker_connection)
    : broker_connection_(broker_connection) {}

void Dispatcher::RunDispatcher() {
    while (true) {
        PollComputableTasks();

        // ReadReply
        // if reply update calculation data
    }
}

void Dispatcher::PollComputableTasks() {
    std::lock_guard l(m_);
    for (auto& [problem_id, calc_data] : problem_id_to_calculation_data_) {
        for (const auto& [task_id, dependencies] :
             calc_data.computable_task_to_dependencies) {
            broker_connection_.SendRequest(
                FormTaskRequest(problem_id, task_id, dependencies));
        }

        calc_data.computable_task_to_dependencies.clear();
    }
}

ProblemId Dispatcher::CalculateProblem(dcmp::AST ast) {
    SyncData sync_data{};

    CalculationData calc_data(dcmp::TaskDecompositor::New(std::move(ast)));

    for (const auto& [task_id, expr_data] :
         calc_data.decompositor.GetIndependentTasks()) {
        DataId data_id;

        data_id.name = std::get<domain::VariableId>(expr_data);
        data_id.is_scalar = IsScalarVariable(data_id.name);

        calc_data.computed_tasks[task_id] = data_id;
    }

    std::unordered_set<dcmp::VertexDescriptor> computed_tasks;

    for (const auto& [task_id, _] : calc_data.computed_tasks) {
        computed_tasks.emplace(task_id);
    }

    calc_data.computable_task_to_dependencies =
        calc_data.decompositor.GetComputableTasks(computed_tasks);

    ProblemId problem_id;

    {
        std::lock_guard<std::mutex> l(m_);
        problem_id = PROBLEM_ID;
        problem_id_to_calculation_data_.emplace(problem_id,
                                                std::move(calc_data));
        problem_id_to_sync_data_.emplace(problem_id, sync_data);
        ++PROBLEM_ID;
        // Pass to Calculator and block mutex then release
        // it when computation is done.
    }

    spdlog::info("Manager got problem with id {}", problem_id);

    {
        std::unique_lock cv_ul(sync_data.cv_m);
        sync_data.cv.wait(cv_ul, [&] { return sync_data.is_computed; });
    }

    // TODO: 1. Read result from storage
    //       2. Lock mutex and remove this problems data from dispatcher
    //       3. Notify storage that intermediate results can be deleted

    return PROBLEM_ID - 1;
}

std::string Dispatcher::FormTaskRequest(
    ProblemId problem_id, dcmp::VertexDescriptor task_id,
    const std::vector<dcmp::VertexDescriptor>& dependencies) const {
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

    return task.SerializeAsString();
}

task::Operand Dispatcher::FormOperand(ProblemId problem_id,
                                      dcmp::VertexDescriptor operand_id) const {
    task::Operand operand;

    NodeResultId node_result = problem_id_to_calculation_data_.at(problem_id)
                                   .computed_tasks.at(operand_id);

    if (std::holds_alternative<NodeId>(node_result)) {
        auto id = new task::TaskId;
        id->set_problem_id(problem_id);
        id->set_task_id(operand_id);

        task::DynOperand* dyn_operand = new task::DynOperand;
        dyn_operand->set_allocated_id(id);
        dyn_operand->set_is_scalar(std::get<NodeId>(node_result).is_scalar);

        operand.set_allocated_dyn_operand(dyn_operand);
    } else if (std::holds_alternative<DataId>(node_result)) {
        task::StaticOperand* static_operand = new task::StaticOperand;
        static_operand->set_identifier(std::get<DataId>(node_result).name);
        static_operand->set_is_scalar(std::get<DataId>(node_result).is_scalar);

        operand.set_allocated_static_operand(static_operand);
    } else {
        assert(false);
    }

    return operand;
}

bool Dispatcher::IsScalarVariable(const std::string& variable_id) {
    return std::all_of(variable_id.begin(), variable_id.end(),
                       [](char c) { return std::islower(c); });
}

Dispatcher::CalculationData::CalculationData(
    dcmp::TaskDecompositor task_decompositor)
    : decompositor(task_decompositor) {}
