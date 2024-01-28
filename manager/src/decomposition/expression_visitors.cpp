#include <magic_enum/magic_enum_iostream.hpp>

#include "expression_visitors.h"

namespace dcmp {

PrintVisitor::PrintVisitor(std::ostream& output) : output_(output) {}

void PrintVisitor::Visit(const Expr& expr) {
    using magic_enum::iostream_operators::operator<<;
    output_ << expr.GetType() << std::endl;
}

}  // namespace dcmp
