#include <algorithm>
#include <memory>
#include <typeinfo>

#include "ast.h"

namespace dcmp {

AST::AST(std::unique_ptr<Expr> root) : root_(std::move(root)) {}

ExprResult AST::Evaluate() const { return root_->Evaluate(); }

void AST::Print(std::ostream& out) const { root_->Print(out); }

}  // namespace dcmp
