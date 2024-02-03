#pragma once
#include <condition_variable>
#include <mutex>
#include <unordered_map>
#include <unordered_set>

#include "decomposition/ast.h"
#include "decomposition/decomposition.h"
#include "dispatch/task_dealer.h"
#include "domain/domain.h"

using ProblemId = size_t;
using TaskId = dcmp::VertexDescriptor;

struct NodeId {
    ProblemId problem_id;
    TaskId task_id;
};

struct DataId {
    domain::VariableId name;
};

using NodeResultId = std::variant<NodeId, DataId>;

class Dispatcher {
   public:
    Dispatcher(TaskDealer& requester);

    void RunDispatcher();

    ProblemId CalculateProblem(dcmp::AST ast);

   private:
    struct CalculationData {
        CalculationData(dcmp::TaskDecompositor task_decompositor);

        dcmp::TaskDecompositor decompositor;
        std::unordered_map<dcmp::VertexDescriptor, NodeResultId> computed_tasks;
        std::unordered_set<dcmp::VertexDescriptor> computable_tasks;
    };

    struct SyncData {
        bool is_computed = false;
        std::condition_variable cv{};
        std::mutex cv_m{};
    };

    std::map<ProblemId, CalculationData> problem_id_to_calculation_data_{};
    std::unordered_map<ProblemId, SyncData&> problem_id_to_sync_data_{};

    TaskDealer& requester_;

    static ProblemId PROBLEM_ID;
    std::mutex m_;
};
