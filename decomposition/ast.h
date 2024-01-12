#pragma once

#include <memory>

#include "expression.h"

namespace dcmp {

class AST {
   public:
    explicit AST(std::unique_ptr<Expr> root);

    AST(AST&&) = default;

    AST& operator=(AST&&) = default;

    ExprResult Evaluate() const;

    void PostorderTraverse(ExprVisitor& visitor) const;

   private:
    std::unique_ptr<Expr> root_;
};

}  // namespace dcmp
