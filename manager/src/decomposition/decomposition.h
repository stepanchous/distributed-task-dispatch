#pragma once

#include <iostream>
#include <vector>

#include "decomposition/ast.h"
#include "decomposition/dag.h"

namespace dcmp {

class TaskDecompositor {
   public:
    static TaskDecompositor New(dcmp::AST ast);

    void PrintTasks(std::ostream& output) const;

   private:
    TaskDecompositor(dcmp::Graph graph,
                     std::vector<dcmp::VertexDescriptor> tasks);

    dcmp::Graph graph_;
    std::vector<dcmp::VertexDescriptor> tasks_;
};

}  // namespace dcmp
