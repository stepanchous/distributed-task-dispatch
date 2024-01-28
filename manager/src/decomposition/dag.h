#pragma once

#include <boost/graph/adjacency_list.hpp>

#include "decomposition/expression.h"

namespace dcmp {

struct VertexProperties {
    dcmp::ExprType type;
    ExprData data;

    bool operator==(const VertexProperties& rhs) const = default;
};

using Graph = boost::adjacency_list<boost::vecS, boost::vecS, boost::directedS,
                                    VertexProperties>;

using VertexDescriptor = boost::graph_traits<Graph>::vertex_descriptor;

}  // namespace dcmp
