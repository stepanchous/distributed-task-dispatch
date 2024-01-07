#pragma once

#include <memory>
#include <optional>
#include <ostream>
#include <variant>

#include "domain/domain.h"

namespace dcmp {

using ExprResult = std::variant<domain::List, domain::Scalar>;

enum ExprType {
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
    list,
    scalar,
};

struct ExprHasher;

class Expr {
   public:
    static std::unique_ptr<Expr> New(
        ExprType, std::unique_ptr<Expr> operand1 = nullptr,
        std::unique_ptr<Expr> operand2 = nullptr,
        std::optional<domain::List> l = std::nullopt,
        std::optional<domain::Scalar> x = std::nullopt);

    virtual ~Expr() = default;
    virtual ExprResult Evaluate() const = 0;
    virtual void Print(std::ostream& out) const = 0;

    virtual bool operator==(const Expr& rhs) const = 0;

   protected:
    virtual size_t Hash() const = 0;

    friend ExprHasher;
};

struct ExprHasher {
    size_t operator()(const Expr& expr) const;
};

struct ExprEqual {
    bool operator()(const Expr& lhs, const Expr& rhs) const;
};

}  // namespace dcmp
