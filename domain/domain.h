#pragma once

#include <type_traits>
#include <variant>
#include <vector>

namespace domain {

using Scalar = int;
using List = std::vector<Scalar>;
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

Scalar Min(const Scalar& x1, const Scalar& x2);
Scalar Max(const Scalar& x1, const Scalar& x2);

}  // namespace domain
