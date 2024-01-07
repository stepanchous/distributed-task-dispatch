#include <magic_enum/magic_enum.hpp>
#include <magic_enum/magic_enum_iostream.hpp>
#include <typeinfo>

#include "domain/domain.h"
#include "expression.h"

namespace dcmp {

constexpr size_t HASH_CONST = 47;

size_t ExprHasher::operator()(const Expr& expr) const {
    return typeid(expr).hash_code() + HASH_CONST * expr.Hash();
}

bool ExprEqual::operator()(const Expr& lhs, const Expr& rhs) const {
    return lhs.operator==(rhs);
}

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

    ExprResult Evaluate() const override {
        domain::Scalar result{};

        switch (type_) {
            case Type::min:
                result =
                    domain::Min(std::get<domain::List>(operand_->Evaluate()));
                break;
            case Type::max:
                result =
                    domain::Max(std::get<domain::List>(operand_->Evaluate()));
                break;
            case Type::mul:
                result =
                    domain::Mul(std::get<domain::List>(operand_->Evaluate()));
                break;
            case Type::sum:
                result =
                    domain::Sum(std::get<domain::List>(operand_->Evaluate()));
                break;
            case Type::size:
                result =
                    domain::Size(std::get<domain::List>(operand_->Evaluate()));
                break;
        }

        return result;
    }

    void Print(std::ostream& out) const override {
        using magic_enum::iostream_operators::operator<<;
        operand_->Print(out);
        out << type_ << std::endl;
    }

    bool operator==(const Expr& rhs) const override {
        if (typeid(*this) != typeid(rhs)) {
            return false;
        }

        const auto& casted_rhs = dynamic_cast<const ReduceExpr&>(rhs);

        return this->type_ == casted_rhs.type_ &&
               *this->operand_ == *casted_rhs.operand_;
    }

    size_t Hash() const override {
        return static_cast<size_t>(type_) +
               HASH_CONST * ExprHasher{}(*operand_);
    }

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

    ExprResult Evaluate() const override {
        domain::List result;

        switch (type_) {
            case Type::add:
                result =
                    domain::Sum(std::get<domain::List>(lhs_->Evaluate()),
                                std::get<domain::Scalar>(rhs_->Evaluate()));
                break;
            case Type::mul:
                result =
                    domain::Mul(std::get<domain::List>(lhs_->Evaluate()),
                                std::get<domain::Scalar>(rhs_->Evaluate()));
                break;
            case Type::div:
                result =
                    domain::Div(std::get<domain::List>(lhs_->Evaluate()),
                                std::get<domain::Scalar>(rhs_->Evaluate()));
                break;
        }

        return result;
    }

    void Print(std::ostream& out) const override {
        using magic_enum::iostream_operators::operator<<;
        lhs_->Print(out);
        rhs_->Print(out);
        out << type_ << std::endl;
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

   private:
    Type type_;
    std::unique_ptr<Expr> lhs_;
    std::unique_ptr<Expr> rhs_;

    size_t Hash() const override {
        return static_cast<size_t>(type_) +
               HASH_CONST *
                   (ExprHasher{}(*lhs_) + HASH_CONST * ExprHasher{}(*rhs_));
    }
};

class ListOpExpr final : public Expr {
   public:
    enum class Type { add, mul, div, dot };

    explicit ListOpExpr(Type type, std::unique_ptr<Expr> lhs,
                        std::unique_ptr<Expr> rhs)
        : type_(type), lhs_(std::move(lhs)), rhs_(std::move(rhs)) {}

    ExprResult Evaluate() const override {
        domain::ExprResult result;

        switch (type_) {
            case Type::add:
                result = domain::Sum(std::get<domain::List>(lhs_->Evaluate()),
                                     std::get<domain::List>(rhs_->Evaluate()));
                break;
            case Type::mul:
                result = domain::Mul(std::get<domain::List>(lhs_->Evaluate()),
                                     std::get<domain::List>(rhs_->Evaluate()));
                break;
            case Type::div:
                result = domain::Div(std::get<domain::List>(lhs_->Evaluate()),
                                     std::get<domain::List>(rhs_->Evaluate()));
                break;
            case Type::dot:
                result = domain::Dot(std::get<domain::List>(lhs_->Evaluate()),
                                     std::get<domain::List>(rhs_->Evaluate()));
                break;
        }

        return result;
    }

    void Print(std::ostream& out) const override {
        using magic_enum::iostream_operators::operator<<;
        lhs_->Print(out);
        rhs_->Print(out);
        out << type_ << std::endl;
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

   private:
    Type type_;
    std::unique_ptr<Expr> lhs_;
    std::unique_ptr<Expr> rhs_;

    size_t Hash() const override {
        return static_cast<size_t>(type_) +
               HASH_CONST *
                   (ExprHasher{}(*lhs_) + HASH_CONST * ExprHasher{}(*rhs_));
    }
};

class ListExpr final : public Expr {
   public:
    explicit ListExpr(const domain::List& l) : l_(l) {}

    ExprResult Evaluate() const override { return l_; }

    void Print(std::ostream& out) const override { out << "List" << std::endl; }

    bool operator==(const Expr& rhs) const override {
        if (typeid(*this) != typeid(rhs)) {
            return false;
        }

        return this->l_ == dynamic_cast<const ListExpr&>(rhs).l_;
    }

   private:
    domain::List l_{};

    size_t Hash() const override { return domain::ListHasher{}(l_); }
};

class ScalarExpr final : public Expr {
   public:
    explicit ScalarExpr(const domain::Scalar& x) : x_(x) {}

    ExprResult Evaluate() const override { return x_; }

    void Print(std::ostream& out) const override {
        out << "Scalar" << std::endl;
    }

    bool operator==(const Expr& rhs) const override {
        if (typeid(*this) != typeid(rhs)) {
            return false;
        }

        return this->x_ == dynamic_cast<const ScalarExpr&>(rhs).x_;
    }

   private:
    domain::Scalar x_{};

    size_t Hash() const override { return std::hash<domain::Scalar>{}(x_); }
};

std::unique_ptr<Expr> Expr::New(ExprType type, std::unique_ptr<Expr> operand1,
                                std::unique_ptr<Expr> operand2,
                                std::optional<domain::List> l,
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
            expr = std::make_unique<ListExpr>(std::move(l.value()));
            break;
        case scalar:
            expr = std::make_unique<ScalarExpr>(std::move(x.value()));
            break;
    }

    return expr;
}

}  // namespace dcmp
