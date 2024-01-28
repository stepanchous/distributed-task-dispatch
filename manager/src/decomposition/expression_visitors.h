#pragma once

#include <iostream>

#include "expression.h"

namespace dcmp {

class PrintVisitor final : public ExprVisitor {
   public:
    PrintVisitor(std::ostream& output);
    void Visit(const Expr& expr) override;

   private:
    std::ostream& output_;
};

}  // namespace dcmp
