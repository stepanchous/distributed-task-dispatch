#pragma once

#include <memory>

#include "expression.h"

namespace dcmp {

class AST {
   public:
    static AST FromJson(std::istream& input);

    AST(const AST&) = delete;

    AST& operator=(const AST&) = delete;

    AST(AST&&) = default;

    AST& operator=(AST&&) = default;

    void PostorderTraverse(ExprVisitor& visitor) const;

   private:
    explicit AST(std::unique_ptr<Expr> root);

    std::unique_ptr<Expr> root_;
};

}  // namespace dcmp
