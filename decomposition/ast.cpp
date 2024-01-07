#include <algorithm>
#include <memory>
#include <typeinfo>

#include "ast.h"

namespace dcmp {

AST::AST(std::unique_ptr<Expr> root) : root_(std::move(root)) {}

}  // namespace dcmp
