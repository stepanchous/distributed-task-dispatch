#pragma once

#include <functional>
#include <memory>
#include <unordered_set>

#include "decomposition/expression.h"

namespace dcmp {

class DAG {
   private:
    std::unordered_set<std::unique_ptr<Expr>> vertecies_;
    std::unordered_map<std::reference_wrapper<Expr>,
                       std::unordered_set<std::reference_wrapper<Expr>>,
                       ExprHasher, ExprEqual>
        adjacency_matrix;
};

}  // namespace dcmp
