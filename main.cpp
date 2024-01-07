#include <iostream>
#include <unordered_set>

#include "decomposition/expression.h"
#include "domain/domain.h"

using namespace dcmp;

int main() {
    auto expr1 =
        Expr::New(ExprType::list, nullptr, nullptr, domain::List{1, 2, 3});
    auto expr2 = Expr::New(ExprType::scalar, nullptr, nullptr, std::nullopt,
                           domain::Scalar{1});
    auto expr3 =
        Expr::New(ExprType::me_add, std::move(expr1), std::move(expr2));
    auto root1 = Expr::New(ExprType::re_sum, std::move(expr3));

    auto expr4 =
        Expr::New(ExprType::list, nullptr, nullptr, domain::List{1, 2, 3});
    auto expr5 = Expr::New(ExprType::scalar, nullptr, nullptr, std::nullopt,
                           domain::Scalar{1});
    auto expr6 =
        Expr::New(ExprType::me_add, std::move(expr4), std::move(expr5));
    auto root2 = Expr::New(ExprType::re_sum, std::move(expr6));

    std::unordered_set<std::reference_wrapper<Expr>, ExprHasher, ExprEqual>
        expr_set;

    auto expr7 = Expr::New(ExprType::scalar, nullptr, nullptr, std::nullopt,
                           domain::Scalar{45});

    expr_set.insert(*root1);

    std::cout << expr_set.count(*root2) << std::endl;
    std::cout << expr_set.count(*expr7) << std::endl;

    root1->Print(std::cout);

    // std::cout << std::get<domain::Scalar>(root->Evaluate()) << std::endl;
}
