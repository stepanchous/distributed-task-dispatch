#include <algorithm>
#include <cassert>
#include <numeric>

#include "domain.h"

namespace domain {

size_t ListHasher::operator()(const List& l) const {
    size_t hash = l.size();

    for (const auto& x : l) {
        hash ^= x + 0x9e3779b9 + (hash << 6) + (hash >> 2);
    }

    return hash;
}

Scalar Mul(const List& l) {
    return std::reduce(
        l.begin(), l.end(), Scalar{1},
        [](const Scalar& lhs, const Scalar& rhs) { return lhs * rhs; });
}

Scalar Sum(const List& l) {
    return std::reduce(
        l.begin(), l.end(), Scalar{},
        [](const Scalar& lhs, const Scalar& rhs) { return lhs + rhs; });
}

Scalar Mul(const Scalar& x1, const Scalar& x2) { return x1 * x2; }

Scalar Div(const Scalar& x1, const Scalar& x2) { return x1 / x2; }

Scalar Sum(const Scalar& x1, const Scalar& x2) { return x1 + x2; }

Scalar Min(const List& l) { return *std::min(l.begin(), l.end()); }

Scalar Max(const List& l) { return *std::min(l.begin(), l.end()); }

Scalar Size(const List& l) { return l.size(); }

Scalar Dot(const List& l1, const List& l2) {
    assert(l1.size() == l2.size());
    return std::inner_product(l1.begin(), l1.end(), l2.begin(), Scalar{});
}

List Mul(const List& l, const Scalar& x) {
    List new_l(l.size());
    std::transform(l.begin(), l.end(), new_l.begin(),
                   [&](const Scalar& y) { return y * x; });
    return new_l;
}

List Div(const List& l, const Scalar& x) {
    List new_l(l.size());
    std::transform(l.begin(), l.end(), new_l.begin(),
                   [&](const Scalar& y) { return y / x; });
    return new_l;
}

List Sum(const List& l, const Scalar& x) {
    List new_l(l.size());
    std::transform(l.begin(), l.end(), new_l.begin(),
                   [&](const Scalar& y) { return y + x; });
    return new_l;
}

List Sum(const List& l1, const List& l2) {
    assert(l1.size() == l2.size());

    List new_l(l1.size());

    for (size_t i = 0; i < l1.size(); ++i) {
        new_l[i] = l1[i] + l2[i];
    }

    return new_l;
}

List Mul(const List& l1, const List& l2) {
    assert(l1.size() == l2.size());

    List new_l(l1.size());

    for (size_t i = 0; i < l1.size(); ++i) {
        new_l[i] = l1[i] * l2[i];
    }

    return new_l;
}

List Div(const List& l1, const List& l2) {
    assert(l1.size() == l2.size());

    List new_l(l1.size());

    for (size_t i = 0; i < l1.size(); ++i) {
        new_l[i] = l1[i] / l2[i];
    }

    return new_l;
}

Scalar Min(const Scalar& x1, const Scalar& x2) { return std::min(x1, x2); }
Scalar Max(const Scalar& x1, const Scalar& x2) { return std::max(x1, x2); }

}  // namespace domain
