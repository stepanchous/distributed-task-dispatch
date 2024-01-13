#include <minibench/log_duration.h>

#include <boost/graph/graphviz.hpp>
#include <iostream>
#include <magic_enum/magic_enum_iostream.hpp>

#include "decomposition/ast.h"
#include "decomposition/dag.h"
#include "decomposition/dag_visitor.h"
#include "decomposition/expression.h"
#include "domain/domain.h"

using namespace dcmp;

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

   private:
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

    Name name_;
};

int main() {
    domain::VariableId l = "L";
    domain::VariableId k = "K";

    auto l1 = Expr::New(ExprType::list, nullptr, nullptr, l);
    auto l2 = Expr::New(ExprType::list, nullptr, nullptr, l);
    auto l3 = Expr::New(ExprType::list, nullptr, nullptr, l);
    auto l4 = Expr::New(ExprType::list, nullptr, nullptr, l);

    auto k1 = Expr::New(ExprType::list, nullptr, nullptr, k);

    auto sum1 = Expr::New(ExprType::re_sum, std::move(l1));
    auto prod1 = Expr::New(ExprType::re_mul, std::move(l2));
    auto sum2 = Expr::New(ExprType::re_sum, std::move(l3));
    auto prod2 = Expr::New(ExprType::re_mul, std::move(l4));

    auto min = Expr::New(ExprType::so_min, std::move(sum2), std::move(prod2));
    auto max = Expr::New(ExprType::so_max, std::move(sum1), std::move(prod1));

    auto add = Expr::New(ExprType::so_add, std::move(max), std::move(min));

    auto mul = Expr::New(ExprType::me_mul, std::move(k1), std::move(add));

    // auto expr3 =
    //     Expr::New(ExprType::me_add, std::move(expr1), std::move(expr2));
    // auto root1 = Expr::New(ExprType::re_sum, std::move(expr3));
    //
    // auto expr4 =
    //     Expr::New(ExprType::list, nullptr, nullptr, l);
    // auto expr5 = Expr::New(ExprType::scalar, nullptr, nullptr,
    // std::nullopt,
    //                        domain::Scalar{1});
    // auto expr6 =
    //     Expr::New(ExprType::me_add, std::move(expr4), std::move(expr5));
    // auto root2 = Expr::New(ExprType::re_sum, std::move(expr6));
    //
    // std::unordered_set<std::reference_wrapper<Expr>, ExprHasher,
    // ExprEqual>
    //     expr_set;
    //
    // auto expr7 = Expr::New(ExprType::scalar, nullptr, nullptr,
    // std::nullopt,
    //                        domain::Scalar{45});
    //
    // expr_set.insert(*root1);
    //
    // std::cout << expr_set.count(*root2) << std::endl;
    // std::cout << expr_set.count(*expr7) << std::endl;

    AST ast(std::move(mul));

    DagVisitor dag_visitor;

    ast.PostorderTraverse(dag_visitor);

    Graph graph = dag_visitor.BuildGraph();

    auto writer = MyLabelWriter{graph};

    boost::write_graphviz(std::cout, graph, writer);
}
