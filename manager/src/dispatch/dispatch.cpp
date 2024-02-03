#include <unistd.h>

#include <condition_variable>
#include <mutex>
#include <utility>

#include "dispatch.h"
#include "domain/domain.h"

ProblemId Dispatcher::PROBLEM_ID = 0;

Dispatcher::Dispatcher(TaskDealer& requester) : requester_(requester) {}

void Dispatcher::RunDispatcher() {
    while (true) {
        {
            std::lock_guard l(m_);
            for (const auto& [problem_id, calc_data] :
                 problem_id_to_calculation_data_) {
                if (calc_data.computable_tasks.empty()) {
                    continue;
                }

                // Send task to broker
            }
        }

        // ReadReply
        // if reply update calculation data
    }
}

ProblemId Dispatcher::CalculateProblem(dcmp::AST ast) {
    SyncData sync_data{};

    CalculationData calc_data(dcmp::TaskDecompositor::New(std::move(ast)));

    for (const auto& [task_id, expr_data] :
         calc_data.decompositor.GetIndependentTasks()) {
        calc_data.computed_tasks[task_id] = DataId{
            .name = std::get<domain::VariableId>(expr_data),
        };
    }

    std::unordered_set<dcmp::VertexDescriptor> computed_tasks;

    for (const auto& [task_id, _] : calc_data.computed_tasks) {
        computed_tasks.emplace(task_id);
    }

    calc_data.computable_tasks =
        calc_data.decompositor.GetComputableTasks(computed_tasks);

    {
        std::lock_guard<std::mutex> l(m_);
        ProblemId problem_id = PROBLEM_ID;
        problem_id_to_calculation_data_.emplace(problem_id,
                                                std::move(calc_data));
        problem_id_to_sync_data_.emplace(problem_id, sync_data);
        ++PROBLEM_ID;
        // Pass to Calculator and block mutex then release
        // it when computation is done.
    }

    std::cout << "Got problem " << PROBLEM_ID - 1 << std::endl;

    {
        std::unique_lock cv_ul(sync_data.cv_m);
        sync_data.cv.wait(cv_ul, [&] { return sync_data.is_computed; });
    }

    // TODO: 1. Read result from storage
    //       2. Lock mutex and remove this problems data from dispatcher
    //       3. Notify storage that intermediate results can be deleted

    return PROBLEM_ID - 1;
}

Dispatcher::CalculationData::CalculationData(
    dcmp::TaskDecompositor task_decompositor)
    : decompositor(task_decompositor) {}
