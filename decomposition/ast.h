#pragma once

#include <memory>

#include "expression.h"

namespace dcmp {

class AST {
   public:
    explicit AST(std::unique_ptr<Expr> root);

    AST(const AST&) = delete;

    AST& operator=(const AST&) = delete;

    AST(AST&&) = default;

    AST& operator=(AST&&) = default;

    void PostorderTraverse(ExprVisitor& visitor) const;

   private:
    std::unique_ptr<Expr> root_;
};

}  // namespace dcmp
