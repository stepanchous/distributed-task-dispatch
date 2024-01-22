#include <magic_enum/magic_enum.hpp>
#include <magic_enum/magic_enum_iostream.hpp>
#include <nlohmann/json.hpp>
#include <stdexcept>

#include "domain/domain.h"
#include "expression.h"

namespace dcmp {

class ReduceExpr final : public Expr {
   public:
    enum class Type {
        min,
        max,
        mul,
        sum,
        size,
    };

    explicit ReduceExpr(Type type, std::unique_ptr<Expr> operand)
        : type_(type), operand_(std::move(operand)) {}

    ExprType GetType() const override {
        ExprType type;

        switch (type_) {
            case Type::min:
                type = re_min;
                break;
            case Type::max:
                type = re_max;
                break;
            case Type::mul:
                type = re_mul;
                break;
            case Type::sum:
                type = re_sum;
                break;
            case Type::size:
                type = re_size;
                break;
            default:
                type = invalid;
                break;
        }

        return type;
    }

    bool operator==(const Expr& rhs) const override {
        if (typeid(*this) != typeid(rhs)) {
            return false;
        }

        const auto& casted_rhs = dynamic_cast<const ReduceExpr&>(rhs);

        return this->type_ == casted_rhs.type_ &&
               *this->operand_ == *casted_rhs.operand_;
    }

    void PostorderTraverse(ExprVisitor& visitor) const override {
        operand_->PostorderTraverse(visitor);
        visitor.Visit(*this);
    }

    const Expr* GetLhs() const override { return operand_.get(); }

    const Expr* GetRhs() const override { return nullptr; }

    ExprData GetData() const override { return std::monostate{}; }

   private:
    Type type_;
    std::unique_ptr<Expr> operand_;
};

class MapExpr final : public Expr {
   public:
    enum class Type {
        add,
        mul,
        div,
    };

    explicit MapExpr(Type type, std::unique_ptr<Expr> lhs,
                     std::unique_ptr<Expr> rhs)
        : type_(type), lhs_(std::move(lhs)), rhs_(std::move(rhs)) {}

    ExprType GetType() const override {
        ExprType type;

        switch (type_) {
            case Type::mul:
                type = me_mul;
                break;
            case Type::add:
                type = me_add;
                break;
            case Type::div:
                type = me_div;
                break;
            default:
                type = invalid;
                break;
        }

        return type;
    }

    void PostorderTraverse(ExprVisitor& visitor) const override {
        lhs_->PostorderTraverse(visitor);
        rhs_->PostorderTraverse(visitor);
        visitor.Visit(*this);
    }

    bool operator==(const Expr& rhs) const override {
        if (typeid(*this) != typeid(rhs)) {
            return false;
        }

        const auto& casted_rhs = dynamic_cast<const MapExpr&>(rhs);

        return this->type_ == casted_rhs.type_ &&
               *this->lhs_ == *casted_rhs.lhs_ &&
               *this->rhs_ == *casted_rhs.rhs_;
    }

    const Expr* GetLhs() const override { return lhs_.get(); }

    const Expr* GetRhs() const override { return rhs_.get(); }

    ExprData GetData() const override { return std::monostate{}; }

   private:
    Type type_;
    std::unique_ptr<Expr> lhs_;
    std::unique_ptr<Expr> rhs_;
};

class ListOpExpr final : public Expr {
   public:
    enum class Type { add, mul, div, dot };

    explicit ListOpExpr(Type type, std::unique_ptr<Expr> lhs,
                        std::unique_ptr<Expr> rhs)
        : type_(type), lhs_(std::move(lhs)), rhs_(std::move(rhs)) {}

    ExprType GetType() const override {
        ExprType type;

        switch (type_) {
            case Type::mul:
                type = lo_mul;
                break;
            case Type::add:
                type = lo_add;
                break;
            case Type::div:
                type = lo_div;
                break;
            case Type::dot:
                type = lo_dot;
                break;
            default:
                type = invalid;
                break;
        }

        return type;
    }

    void PostorderTraverse(ExprVisitor& visitor) const override {
        lhs_->PostorderTraverse(visitor);
        rhs_->PostorderTraverse(visitor);
        visitor.Visit(*this);
    }

    bool operator==(const Expr& rhs) const override {
        if (typeid(*this) != typeid(rhs)) {
            return false;
        }

        const auto& casted_rhs = dynamic_cast<const ListOpExpr&>(rhs);

        return this->type_ == casted_rhs.type_ &&
               *this->lhs_ == *casted_rhs.lhs_ &&
               *this->rhs_ == *casted_rhs.rhs_;
    }

    const Expr* GetLhs() const override { return lhs_.get(); }

    const Expr* GetRhs() const override { return rhs_.get(); }

    ExprData GetData() const override { return std::monostate{}; }

   private:
    Type type_;
    std::unique_ptr<Expr> lhs_;
    std::unique_ptr<Expr> rhs_;
};

class ScalarOpExpr final : public Expr {
   public:
    enum class Type { add, mul, div, max, min };

    explicit ScalarOpExpr(Type type, std::unique_ptr<Expr> lhs,
                          std::unique_ptr<Expr> rhs)
        : type_(type), lhs_(std::move(lhs)), rhs_(std::move(rhs)) {}

    ExprType GetType() const override {
        ExprType type;

        switch (type_) {
            case Type::mul:
                type = so_mul;
                break;
            case Type::add:
                type = so_add;
                break;
            case Type::div:
                type = so_div;
                break;
            case Type::max:
                type = so_max;
                break;
            case Type::min:
                type = so_min;
                break;
            default:
                type = invalid;
                break;
        }

        return type;
    }

    void PostorderTraverse(ExprVisitor& visitor) const override {
        lhs_->PostorderTraverse(visitor);
        rhs_->PostorderTraverse(visitor);
        visitor.Visit(*this);
    }

    bool operator==(const Expr& rhs) const override {
        if (typeid(*this) != typeid(rhs)) {
            return false;
        }

        const auto& casted_rhs = dynamic_cast<const ScalarOpExpr&>(rhs);

        return this->type_ == casted_rhs.type_ &&
               *this->lhs_ == *casted_rhs.lhs_ &&
               *this->rhs_ == *casted_rhs.rhs_;
    }

    const Expr* GetLhs() const override { return lhs_.get(); }

    const Expr* GetRhs() const override { return rhs_.get(); }

    ExprData GetData() const override { return std::monostate{}; }

   private:
    Type type_;
    std::unique_ptr<Expr> lhs_;
    std::unique_ptr<Expr> rhs_;
};

class ListExpr final : public Expr {
   public:
    explicit ListExpr(const domain::VariableId& l) : l_(l) {}

    ExprType GetType() const override { return ExprType::list; }

    void PostorderTraverse(ExprVisitor& visitor) const override {
        visitor.Visit(*this);
    }

    bool operator==(const Expr& rhs) const override {
        if (typeid(*this) != typeid(rhs)) {
            return false;
        }

        return this->l_ == dynamic_cast<const ListExpr&>(rhs).l_;
    }

    const Expr* GetLhs() const override { return nullptr; }

    const Expr* GetRhs() const override { return nullptr; }

    ExprData GetData() const override { return l_; }

   private:
    domain::VariableId l_{};
};

class ScalarConstExpr final : public Expr {
   public:
    explicit ScalarConstExpr(const domain::Scalar& x) : x_(x) {}

    ExprType GetType() const override { return ExprType::scalar_const; }

    void PostorderTraverse(ExprVisitor& visitor) const override {
        visitor.Visit(*this);
    }

    bool operator==(const Expr& rhs) const override {
        if (typeid(*this) != typeid(rhs)) {
            return false;
        }

        return this->x_ == dynamic_cast<const ScalarConstExpr&>(rhs).x_;
    }

    const Expr* GetLhs() const override { return nullptr; }

    const Expr* GetRhs() const override { return nullptr; }

    ExprData GetData() const override { return x_; }

   private:
    domain::Scalar x_{};
};

class ScalarVarExpr final : public Expr {
   public:
    explicit ScalarVarExpr(const domain::VariableId& x) : x_(x) {}

    ExprType GetType() const override { return ExprType::scalar_var; }

    void PostorderTraverse(ExprVisitor& visitor) const override {
        visitor.Visit(*this);
    }

    bool operator==(const Expr& rhs) const override {
        if (typeid(*this) != typeid(rhs)) {
            return false;
        }

        return this->x_ == dynamic_cast<const ScalarVarExpr&>(rhs).x_;
    }

    const Expr* GetLhs() const override { return nullptr; }

    const Expr* GetRhs() const override { return nullptr; }

    ExprData GetData() const override { return x_; }

   private:
    domain::VariableId x_{};
};

std::unique_ptr<Expr> Expr::New(ExprType type, std::unique_ptr<Expr> operand1,
                                std::unique_ptr<Expr> operand2,
                                std::optional<domain::VariableId> var_id,
                                std::optional<domain::Scalar> x) {
    std::unique_ptr<Expr> expr = nullptr;

    switch (type) {
        case re_min:
            expr = std::make_unique<ReduceExpr>(ReduceExpr::Type::min,
                                                std::move(operand1));
            break;
        case re_max:
            expr = std::make_unique<ReduceExpr>(ReduceExpr::Type::max,
                                                std::move(operand1));
            break;
        case re_mul:
            expr = std::make_unique<ReduceExpr>(ReduceExpr::Type::mul,
                                                std::move(operand1));
            break;
        case re_sum:
            expr = std::make_unique<ReduceExpr>(ReduceExpr::Type::sum,
                                                std::move(operand1));
            break;
        case re_size:
            expr = std::make_unique<ReduceExpr>(ReduceExpr::Type::size,
                                                std::move(operand1));
            break;
        case me_add:
            expr = std::make_unique<MapExpr>(
                MapExpr::Type::add, std::move(operand1), std::move(operand2));
            break;
        case me_mul:
            expr = std::make_unique<MapExpr>(
                MapExpr::Type::mul, std::move(operand1), std::move(operand2));
            break;
        case me_div:
            expr = std::make_unique<MapExpr>(
                MapExpr::Type::div, std::move(operand1), std::move(operand2));
            break;
        case lo_add:
            expr = std::make_unique<ListOpExpr>(ListOpExpr::Type::add,
                                                std::move(operand1),
                                                std::move(operand2));
            break;
        case lo_mul:
            expr = std::make_unique<ListOpExpr>(ListOpExpr::Type::mul,
                                                std::move(operand1),
                                                std::move(operand2));
            break;
        case lo_div:
            expr = std::make_unique<ListOpExpr>(ListOpExpr::Type::div,
                                                std::move(operand1),
                                                std::move(operand2));
            break;
        case lo_dot:
            expr = std::make_unique<ListOpExpr>(ListOpExpr::Type::dot,
                                                std::move(operand1),
                                                std::move(operand2));
            break;
        case list:
            expr = std::make_unique<ListExpr>(
                std::move(std::move(var_id.value())));
            break;
        case scalar_const:
            expr = std::make_unique<ScalarConstExpr>(std::move(x.value()));
            break;
        case scalar_var:
            expr = std::make_unique<ScalarVarExpr>(std::move(var_id.value()));
            break;
        case so_add:
            expr = std::make_unique<ScalarOpExpr>(ScalarOpExpr::Type::add,
                                                  std::move(operand1),
                                                  std::move(operand2));
            break;
        case so_mul:
            expr = std::make_unique<ScalarOpExpr>(ScalarOpExpr::Type::mul,
                                                  std::move(operand1),
                                                  std::move(operand2));
            break;
        case so_div:
            expr = std::make_unique<ScalarOpExpr>(ScalarOpExpr::Type::div,
                                                  std::move(operand1),
                                                  std::move(operand2));
            break;
        case so_max:
            expr = std::make_unique<ScalarOpExpr>(ScalarOpExpr::Type::max,
                                                  std::move(operand1),
                                                  std::move(operand2));
            break;
        case so_min:
            expr = std::make_unique<ScalarOpExpr>(ScalarOpExpr::Type::min,
                                                  std::move(operand1),
                                                  std::move(operand2));
            break;
        case invalid:
            throw std::invalid_argument("Invalid can not be passed to factory");
            break;
    }

    return expr;
}

std::unique_ptr<Expr> FromJsonImpl(const nlohmann::json& json) {
    using namespace nlohmann;
    auto node = json.begin();

    if (node.key() == "list") {
        return Expr::New(list, nullptr, nullptr, node->get<std::string>());
    }

    if (node.key().starts_with("scalar")) {
        if (node.key() == "scalar_var") {
            return Expr::New(scalar_var, nullptr, nullptr,
                             node->get<std::string>());
        } else {
            return Expr::New(scalar_const, nullptr, nullptr, std::nullopt,
                             node->get<domain::Scalar>());
        }
    }

    return Expr::New(
        STRING_TO_EXPR_TYPE.at(node.key()),
        node->contains("lhs") ? FromJsonImpl(node->at("lhs")) : nullptr,
        node->contains("rhs") ? FromJsonImpl(node->at("rhs")) : nullptr);
}

std::unique_ptr<Expr> Expr::FromJson(std::istream& input) {
    using namespace nlohmann;

    auto json_ast = json::parse(input);
    auto root = json_ast.at("ast");

    return FromJsonImpl(root);
}

}  // namespace dcmp
