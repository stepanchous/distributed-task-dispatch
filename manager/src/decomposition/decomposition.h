#pragma once

#include <unordered_set>
#include <vector>

#include "decomposition/ast.h"
#include "decomposition/dag.h"

namespace dcmp {

class TaskDecompositor {
   public:
    static TaskDecompositor New(dcmp::AST ast);

    const dcmp::Graph& GetGraph() const;

    const std::vector<dcmp::VertexDescriptor>& GetSortedTasks() const;

    std::unordered_map<dcmp::VertexDescriptor, domain::VariableId>
    GetIndependentTasks() const;

    std::unordered_map<dcmp::VertexDescriptor,
                       std::vector<dcmp::VertexDescriptor>>
    GetComputableTasks(
        const std::unordered_set<dcmp::VertexDescriptor>& computed_tasks,
        const std::unordered_set<dcmp::VertexDescriptor>& exclude_tasks = {})
        const;

    std::vector<dcmp::VertexDescriptor> GetRequiredDependencies(
        dcmp::VertexDescriptor task) const;

    size_t GetOperationType(dcmp::VertexDescriptor task) const;

    void PrintTasks(std::ostream& output) const;

   private:
    TaskDecompositor(dcmp::Graph graph,
                     std::vector<dcmp::VertexDescriptor> tasks);

    dcmp::Graph graph_;
    std::vector<dcmp::VertexDescriptor> tasks_;
};

}  // namespace dcmp
