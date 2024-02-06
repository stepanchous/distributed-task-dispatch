#pragma once
#include <condition_variable>
#include <mutex>
#include <unordered_map>

#include "database-client/database_client.h"
#include "decomposition/ast.h"
#include "decomposition/decomposition.h"
#include "dispatch/task_dealer.h"
#include "domain/domain.h"
#include "task.pb.h"

class Dispatcher {
   public:
    using ProblemId = size_t;
    using TaskId = dcmp::VertexDescriptor;
    using TaskToVariableIdMap = std::unordered_map<TaskId, domain::VariableId>;
    using TaskToDependenciesMap =
        std::unordered_map<TaskId, std::vector<TaskId>>;

    enum class TaskStatus {
        processing,
        completed,
    };

    Dispatcher(BrokerConnection requester);

    void RunDispatcher();

    domain::ExprResult CalculateProblem(dcmp::AST ast);

   private:
    struct CalculationData {
        static CalculationData New(dcmp::AST ast);

        dcmp::TaskDecompositor decompositor;
        std::unordered_map<TaskId, TaskStatus> processed_tasks_to_status;
        TaskToVariableIdMap task_to_variable_id;
        TaskToDependenciesMap computable_task_to_dependencies;

       private:
        CalculationData(dcmp::TaskDecompositor task_decompositor);
    };

    struct SyncData {
        bool is_computed = false;
        std::condition_variable cv{};
        std::mutex cv_m{};
    };

   private:
    std::map<ProblemId, CalculationData> problem_id_to_calculation_data_{};
    std::unordered_map<ProblemId, SyncData&> problem_id_to_sync_data_{};
    std::mutex m_;
    BrokerConnection broker_connection_;
    DatabaseClient db_client_;
    static ProblemId PROBLEM_ID;

    void SendComputableTasks();

    void UpdateComputableTasks();

    task::Task FormTaskRequest(ProblemId problem_id, TaskId task_id,
                               const std::vector<TaskId>& dependencies) const;

    task::Operand FormOperand(ProblemId problem_id, TaskId operand_id) const;

    void ProcessNewComputedTask(const task::TaskId& task_id);

    void NotifyComputedProblem(ProblemId problem_id, TaskId task_id);

    ProblemId PushProblem(CalculationData calc_data, SyncData& sync_data);

    void ClearProblem(ProblemId problem_id);

    void UpdateComputableTasksImpl(ProblemId problem_id, TaskId task_id);

    static bool IsScalarVariable(const std::string& variable_id);
};
