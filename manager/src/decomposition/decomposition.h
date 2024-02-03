#pragma once

#include <iostream>
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

    std::unordered_map<dcmp::VertexDescriptor, dcmp::ExprData>
    GetIndependentTasks() const;

    std::unordered_set<dcmp::VertexDescriptor> GetComputableTasks(
        const std::unordered_set<dcmp::VertexDescriptor>& computed_tasks) const;

    void PrintTasks(std::ostream& output) const;

   private:
    TaskDecompositor(dcmp::Graph graph,
                     std::vector<dcmp::VertexDescriptor> tasks);

    dcmp::Graph graph_;
    std::vector<dcmp::VertexDescriptor> tasks_;
};

}  // namespace dcmp
