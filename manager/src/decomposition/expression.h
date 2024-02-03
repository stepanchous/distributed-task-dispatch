#pragma once

#include <memory>
#include <optional>
#include <unordered_map>
#include <variant>

#include "domain/domain.h"

namespace dcmp {

using ExprData =
    std::variant<std::monostate, domain::VariableId, domain::Scalar>;


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
    scalar_const,
    scalar_var,
};

static const std::unordered_map<std::string, ExprType> STRING_TO_EXPR_TYPE = {
    {"invalid", invalid},
    {"re_min", re_min},
    {"re_max", re_max},
    {"re_mul", re_mul},
    {"re_sum", re_sum},
    {"re_size", re_size},
    {"me_add", me_add},
    {"me_mul", me_mul},
    {"me_div", me_div},
    {"lo_add", lo_add},
    {"lo_mul", lo_mul},
    {"lo_div", lo_div},
    {"lo_dot", lo_dot},
    {"so_add", so_add},
    {"so_mul", so_mul},
    {"so_div", so_div},
    {"so_max", so_max},
    {"so_min", so_min},
    {"list", list},
    {"scalar_const", scalar_const},
    {"scalar_var", scalar_var},
};

struct ExprHasher;
class ExprVisitor;

class Expr {
   public:
    static std::unique_ptr<Expr> New(
        ExprType, std::unique_ptr<Expr> operand1 = nullptr,
        std::unique_ptr<Expr> operand2 = nullptr,
        std::optional<domain::VariableId> var_id = std::nullopt,
        std::optional<domain::Scalar> x = std::nullopt);

    static std::unique_ptr<Expr> FromJson(std::istream& input);

    virtual ~Expr() = default;
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
