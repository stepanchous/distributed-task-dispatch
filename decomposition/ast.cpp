#include "ast.h"

namespace dcmp {

AST::AST(std::unique_ptr<Expr> root) : root_(std::move(root)) {}

void AST::PostorderTraverse(ExprVisitor& visitor) const {
    root_->PostorderTraverse(visitor);
}

}  // namespace dcmp
