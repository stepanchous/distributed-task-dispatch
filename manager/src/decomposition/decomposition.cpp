#include <algorithm>
#include <boost/graph/topological_sort.hpp>
#include <iterator>
#include <magic_enum/magic_enum_iostream.hpp>

#include "decomposition/dag_visitor.h"
#include "decomposition/decomposition.h"

namespace dcmp {

const dcmp::Graph& TaskDecompositor::GetGraph() const { return graph_; }

const std::vector<dcmp::VertexDescriptor>& TaskDecompositor::GetSortedTasks()
    const {
    return tasks_;
}

TaskDecompositor::TaskDecompositor(dcmp::Graph graph,
                                   std::vector<dcmp::VertexDescriptor> tasks)
    : graph_(std::move(graph)), tasks_(std::move(tasks)) {}

TaskDecompositor TaskDecompositor::New(dcmp::AST ast) {
    DagVisitor dag_visitor;
    ast.PostorderTraverse(dag_visitor);

    dcmp::Graph graph = dag_visitor.BuildGraph();
    std::vector<dcmp::VertexDescriptor> tasks;
    boost::topological_sort(graph, std::back_inserter(tasks));

    return TaskDecompositor(std::move(graph), std::move(tasks));
}

std::unordered_map<dcmp::VertexDescriptor, dcmp::ExprData>
TaskDecompositor::GetIndependentTasks() const {
    std::unordered_map<dcmp::VertexDescriptor, dcmp::ExprData>
        independent_tasks;

    for (const auto& task : tasks_) {
        dcmp::ExprType task_type = graph_[task].type;

        if (task_type == list || task_type == scalar_const ||
            task_type == scalar_var) {
            independent_tasks.emplace(task, graph_[task].data);
        }
    }

    return independent_tasks;
}

std::unordered_set<dcmp::VertexDescriptor> TaskDecompositor::GetComputableTasks(
    const std::unordered_set<dcmp::VertexDescriptor>& computed_tasks) const {
    std::unordered_set<dcmp::VertexDescriptor> computable_tasks;

    for (const auto& task : tasks_) {
        if (!computed_tasks.contains(task)) {
            auto [adjacent_vetices_first, adjacent_vetices_last] =
                boost::adjacent_vertices(task, graph_);
            if (std::all_of(adjacent_vetices_first, adjacent_vetices_last,
                            [&](const auto& vertex) {
                                return computed_tasks.contains(vertex);
                            })) {
                computable_tasks.insert(task);
            }
        }
    }

    return computable_tasks;
}

void TaskDecompositor::PrintTasks(std::ostream& output) const {
    using magic_enum::iostream_operators::operator<<;
    for (const auto& task : tasks_) {
        output << graph_[task].type << std::endl;
    }
}

}  // namespace dcmp
