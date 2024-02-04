#pragma once
#include <condition_variable>
#include <mutex>
#include <unordered_map>

#include "decomposition/ast.h"
#include "decomposition/decomposition.h"
#include "dispatch/task_dealer.h"
#include "domain/domain.h"
#include "task.pb.h"

using ProblemId = size_t;
using TaskId = dcmp::VertexDescriptor;

struct NodeId {
    ProblemId problem_id;
    TaskId task_id;
    bool is_scalar;
};

struct DataId {
    domain::VariableId name;
    bool is_scalar;
};

using NodeResultId = std::variant<NodeId, DataId>;

class Dispatcher {
   public:
    Dispatcher(BrokerConnection& requester);

    void RunDispatcher();

    ProblemId CalculateProblem(dcmp::AST ast);

   private:
    struct CalculationData {
        CalculationData(dcmp::TaskDecompositor task_decompositor);

        dcmp::TaskDecompositor decompositor;
        std::unordered_map<dcmp::VertexDescriptor, NodeResultId> computed_tasks;
        std::unordered_map<dcmp::VertexDescriptor,
                           std::vector<dcmp::VertexDescriptor>>
            computable_task_to_dependencies;
    };

    struct SyncData {
        bool is_computed = false;
        std::condition_variable cv{};
        std::mutex cv_m{};
    };

    void PollComputableTasks();

    task::Task FormTaskRequest(
        ProblemId problem_id, dcmp::VertexDescriptor task_id,
        const std::vector<dcmp::VertexDescriptor>& dependencies) const;

    task::Operand FormOperand(ProblemId problem_id,
                              dcmp::VertexDescriptor operand_id) const;

    static bool IsScalarVariable(const std::string& variable_id);

   private:
    std::map<ProblemId, CalculationData> problem_id_to_calculation_data_{};
    std::unordered_map<ProblemId, SyncData&> problem_id_to_sync_data_{};
    BrokerConnection& broker_connection_;
    static ProblemId PROBLEM_ID;
    std::mutex m_;
};
