#include <boost/graph/topological_sort.hpp>
#include <iterator>
#include <magic_enum/magic_enum_iostream.hpp>

#include "decomposition/dag_visitor.h"
#include "decomposition/decomposition.h"

namespace dcmp {

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

void TaskDecompositor::PrintTasks(std::ostream& output) const {
    using magic_enum::iostream_operators::operator<<;
    for (const auto& task : tasks_) {
        output << graph_[task].type << std::endl;
    }
}

}  // namespace dcmp
