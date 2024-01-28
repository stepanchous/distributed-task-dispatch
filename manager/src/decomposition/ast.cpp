#include <nlohmann/json.hpp>

#include "ast.h"
#include "decomposition/expression.h"

namespace dcmp {

AST AST::FromJson(std::istream& input) { return AST(Expr::FromJson(input)); }

AST::AST(std::unique_ptr<Expr> root) : root_(std::move(root)) {}

void AST::PostorderTraverse(ExprVisitor& visitor) const {
    root_->PostorderTraverse(visitor);
}

}  // namespace dcmp
