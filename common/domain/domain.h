#pragma once

#include <string>
#include <type_traits>
#include <variant>
#include <vector>

namespace domain {

enum class ExprType {
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

using Scalar = int;
using List = std::vector<Scalar>;
using VariableId = std::string;
using ExprResult = std::variant<List, Scalar>;

static_assert(std::is_arithmetic_v<Scalar>, "Scalar must be arithmetic type");

struct ListHasher {
    size_t operator()(const List& l) const;
};

Scalar Mul(const List& l);
Scalar Sum(const List& l);
Scalar Min(const List& l);
Scalar Max(const List& l);
Scalar Size(const List& l);

Scalar Dot(const List& l1, const List& l2);

List Sum(const List& l, const Scalar& x);
List Mul(const List& l, const Scalar& x);
List Div(const List& l, const Scalar& x);

List Sum(const List& l1, const List& l2);
List Mul(const List& l1, const List& l2);
List Div(const List& l1, const List& l2);

Scalar Mul(const Scalar& x1, const Scalar& x2);
Scalar Div(const Scalar& x1, const Scalar& x2);
Scalar Sum(const Scalar& x1, const Scalar& x2);
Scalar Min(const Scalar& x1, const Scalar& x2);
Scalar Max(const Scalar& x1, const Scalar& x2);

}  // namespace domain
