#include <algorithm>
#include <boost/config.hpp>
#include <boost/graph/adjacency_list.hpp>
#include <boost/graph/detail/adjacency_list.hpp>
#include <boost/range/iterator_range_core.hpp>
#include <magic_enum/magic_enum_iostream.hpp>

#include "dag_visitor.h"
#include "decomposition/dag.h"
#include "decomposition/expression.h"

namespace dcmp {

void DagVisitor::Visit(const Expr& expr) {
    const Expr* record = &expr;

    auto it = std::find_if(records_.begin(), records_.end(),
                           [&](const Expr* el) { return *el == expr; });

    if (it != records_.end()) {
        return;
    }

    records_.emplace_back(record);
}

Graph DagVisitor::BuildGraph() const {
    Graph graph;

    auto node = [&graph](const dcmp::VertexProperties& properties) {
        for (auto v : boost::make_iterator_range(boost::vertices(graph))) {
            if (graph[v] == properties) {
                return v;
            }
        }
        return boost::add_vertex(properties, graph);
    };

    for (const Expr* expr : records_) {
        const Expr* lhs = expr->GetLhs();
        const Expr* rhs = expr->GetRhs();

        if (lhs) {
            boost::add_edge(node({expr->GetType(), expr->GetData()}),
                            node({lhs->GetType(), lhs->GetData()}), graph);
        }

        if (rhs) {
            boost::add_edge(node({expr->GetType(), expr->GetData()}),
                            node({rhs->GetType(), rhs->GetData()}), graph);
        }
    }

    return graph;
}

void DagVisitor::PrintRecords(std::ostream& output) const {
    using magic_enum::iostream_operators::operator<<;
    for (const Expr* expr : records_) {
        ExprType type = expr->GetType();
        const Expr* lhs = expr->GetLhs();
        const Expr* rhs = expr->GetRhs();

        output << type;

        if (!lhs) {
            output << "\tnil";
        } else {
            output << '\t' << lhs->GetType();
        }

        if (!rhs) {
            output << "\tnil";
        } else {
            output << '\t' << rhs->GetType();
        }

        output << '\n';
    }
}

}  // namespace dcmp
