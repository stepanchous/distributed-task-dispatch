#pragma once

#include <memory>
#include <optional>
#include <variant>

#include "domain/domain.h"

namespace dcmp {

using ExprResult = std::variant<domain::List, domain::Scalar>;
using ExprData = std::variant<std::monostate, domain::List, domain::Scalar>;

enum ExprType {
    invalid = 0,
    re_min,
    re_max,
    re_mul,
    re_sum,
    re_size,
    me_add,
    me_mul,
    me_div,
    lo_add,
    lo_mul,
    lo_div,
    lo_dot,
    so_add,
    so_mul,
    so_div,
    so_max,
    so_min,
    list,
    scalar,
};

struct ExprHasher;
class ExprVisitor;

class Expr {
   public:
    static std::unique_ptr<Expr> New(
        ExprType, std::unique_ptr<Expr> operand1 = nullptr,
        std::unique_ptr<Expr> operand2 = nullptr,
        std::optional<domain::List> l = std::nullopt,
        std::optional<domain::Scalar> x = std::nullopt);

    virtual ~Expr() = default;
    virtual ExprResult Evaluate() const = 0;
    virtual ExprType GetType() const = 0;
    virtual void PostorderTraverse(ExprVisitor& visitor) const = 0;
    virtual const Expr* GetLhs() const = 0;
    virtual const Expr* GetRhs() const = 0;
    virtual ExprData GetData() const = 0;

    virtual bool operator==(const Expr& rhs) const = 0;
};

class ExprVisitor {
   public:
    virtual ~ExprVisitor() = default;
    virtual void Visit(const Expr& expr) = 0;
};

}  // namespace dcmp
