#include <minibench/log_duration.h>

#include <any>
#include <boost/graph/graphviz.hpp>
#include <boost/graph/subgraph.hpp>
#include <boost/graph/topological_sort.hpp>
#include <iostream>
#include <magic_enum/magic_enum_iostream.hpp>

#include "decomposition/ast.h"
#include "decomposition/dag.h"
#include "decomposition/dag_visitor.h"
#include "domain/domain.h"

using namespace dcmp;
using magic_enum::iostream_operators::operator<<;

template <class Name>
class MyLabelWriter {
   public:
    MyLabelWriter(Name name) : name_(name) {}

    template <class VertexOrEdge>
    void operator()(std::ostream& out, const VertexOrEdge& v) {
        using magic_enum::iostream_operators::operator<<;

        DataVisitor data_visitor(out);

        out << "[label=\"" << name_[v].type;
        std::visit(data_visitor, name_[v].data);
        out << "\"]";
    }

    struct DataVisitor {
        DataVisitor(std::ostream& out) : out_(out) {}

        void operator()(domain::Scalar x) { out_ << '(' << x << ')'; }
        void operator()(const domain::VariableId& var_id) {
            out_ << '(' << var_id << ')';
        }
        void operator()(std::monostate) {}

       private:
        std::ostream& out_;
    };

   private:
    Name name_;
};

int main(int argc, char* argv[]) {
    if (argc != 2) {
        std::cerr << "Invalid number of arguments" << std::endl;
        return -1;
    }

    std::ifstream f(argv[1]);
    if (!f) {
        std::cerr << "Unable to open file" << std::endl;
        return -1;
    }

    auto ast = AST::FromJson(f);

    DagVisitor dag_visitor;
    ast.PostorderTraverse(dag_visitor);

    Graph graph = dag_visitor.BuildGraph();

    boost::write_graphviz(std::cout, graph, MyLabelWriter{graph});

    std::vector<VertexDescriptor> sorted_graph;
    boost::topological_sort(graph, std::back_inserter(sorted_graph));

    for (auto it = sorted_graph.begin(); it != sorted_graph.end(); ++it) {
        MyLabelWriter<std::any>::DataVisitor writer{std::cerr};

        std::cerr << graph[*it].type;
        std::visit(writer, graph[*it].data);
        std::cerr << '\n';
    }
}
