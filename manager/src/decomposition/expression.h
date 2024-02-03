#pragma once

#include <memory>
#include <optional>
#include <unordered_map>
#include <variant>

#include "domain/domain.h"

namespace dcmp {

using ExprData =
    std::variant<std::monostate, domain::VariableId, domain::Scalar>;

static const std::unordered_map<std::string, domain::ExprType>
    STRING_TO_EXPR_TYPE = {
        {"invalid", domain::ExprType::invalid},
        {"re_min", domain::ExprType::re_min},
        {"re_max", domain::ExprType::re_max},
        {"re_mul", domain::ExprType::re_mul},
        {"re_sum", domain::ExprType::re_sum},
        {"re_size", domain::ExprType::re_size},
        {"me_add", domain::ExprType::me_add},
        {"me_mul", domain::ExprType::me_mul},
        {"me_div", domain::ExprType::me_div},
        {"lo_add", domain::ExprType::lo_add},
        {"lo_mul", domain::ExprType::lo_mul},
        {"lo_div", domain::ExprType::lo_div},
        {"lo_dot", domain::ExprType::lo_dot},
        {"so_add", domain::ExprType::so_add},
        {"so_mul", domain::ExprType::so_mul},
        {"so_div", domain::ExprType::so_div},
        {"so_max", domain::ExprType::so_max},
        {"so_min", domain::ExprType::so_min},
        {"list", domain::ExprType::list},
        {"scalar_const", domain::ExprType::scalar_const},
        {"scalar_var", domain::ExprType::scalar_var},
};

struct ExprHasher;
class ExprVisitor;

class Expr {
   public:
    static std::unique_ptr<Expr> New(
        domain::ExprType, std::unique_ptr<Expr> operand1 = nullptr,
        std::unique_ptr<Expr> operand2 = nullptr,
        std::optional<domain::VariableId> var_id = std::nullopt,
        std::optional<domain::Scalar> x = std::nullopt);

    static std::unique_ptr<Expr> FromJson(std::istream& input);

    virtual ~Expr() = default;
    virtual domain::ExprType GetType() const = 0;
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
