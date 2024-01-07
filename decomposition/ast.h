#pragma once

#include <memory>
#include <variant>
#include <vector>

#include "expression.h"

namespace dcmp {

class AST {
   public:
    explicit AST(std::unique_ptr<Expr> root);

    AST(AST&&) = default;

    AST& operator=(AST&&) = default;

    ExprResult Evaluate() const;

    ~AST();

   private:
    std::unique_ptr<Expr> root_;
};

}  // namespace dcmp
