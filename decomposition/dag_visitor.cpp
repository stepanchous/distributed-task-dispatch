#include <algorithm>
#include <boost/config.hpp>
#include <boost/graph/adjacency_list.hpp>
#include <boost/graph/detail/adjacency_list.hpp>
#include <boost/range/iterator_range_core.hpp>
#include <iterator>
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

    std::unordered_map<size_t, VertexDescriptor> id_to_descriptor;

    for (auto it = records_.begin(); it != records_.end(); ++it) {
        const VertexProperties vertex_properties = {.type = (*it)->GetType(),
                                                    .data = (*it)->GetData()};

        const VertexDescriptor descriptor =
            boost::add_vertex(vertex_properties, graph);

        id_to_descriptor[std::distance(records_.begin(), it)] = descriptor;
    }

    auto add_edge = [&](auto root_it, const Expr* operand) {
        const VertexDescriptor root_descriptor =
            id_to_descriptor.at(std::distance(records_.begin(), root_it));

        auto operand_it = std::find_if(
            records_.begin(), records_.end(),
            [operand](const Expr* record) { return *record == *operand; });

        VertexDescriptor operand_descriptor =
            id_to_descriptor.at(std::distance(records_.begin(), operand_it));

        boost::add_edge(root_descriptor, operand_descriptor, graph);
    };

    for (auto expr_it = records_.begin(); expr_it != records_.end();
         ++expr_it) {
        const Expr* lhs = (*expr_it)->GetLhs();
        const Expr* rhs = (*expr_it)->GetRhs();

        if (lhs) {
            add_edge(expr_it, lhs);
        }

        if (rhs) {
            add_edge(expr_it, rhs);
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
