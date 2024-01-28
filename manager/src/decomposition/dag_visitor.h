#pragma once

#include <ostream>

#include "decomposition/dag.h"
#include "expression.h"

namespace dcmp {

class DagVisitor final : public ExprVisitor {
   public:
    using VertexDescriptor = boost::graph_traits<Graph>::vertex_descriptor;

    void Visit(const Expr& expr) override;

    Graph BuildGraph() const;

    void PrintRecords(std::ostream& output) const;

   private:
    std::vector<const Expr*> records_;
};

}  // namespace dcmp
