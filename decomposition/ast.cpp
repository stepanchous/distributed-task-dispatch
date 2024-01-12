#include "ast.h"

namespace dcmp {

AST::AST(std::unique_ptr<Expr> root) : root_(std::move(root)) {}

ExprResult AST::Evaluate() const { return root_->Evaluate(); }

void AST::PostorderTraverse(ExprVisitor& visitor) const {
    root_->PostorderTraverse(visitor);
}

}  // namespace dcmp
