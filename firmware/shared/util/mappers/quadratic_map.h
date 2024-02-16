/// @author Blake Freer
/// @date 2024-01-22

#pragma once

#include "mapper.h"

namespace shared::util {
/**
 * @brief Evaluates the quadratic function `f(x) = axx + bx + c`.
 * @tparam T Ouptut type.
 * @tparam U Input type.
 */
template <typename T, typename U = T>
class QuadraticMap : public Mapper<T, U> {
public:
    QuadraticMap(T a, T b, T c) : a_(a), b_(b), c_(c) {}

    static inline T Evaluate(U x, T a, T b, T c) {
        return a * x * x + b * x + c;
    }

    inline T Evaluate(U x) const override {
        return QuadraticMap<T, U>::Evaluate(x, a_, b_, c_);
    }

private:
    const T a_;
    const T b_;
    const T c_;
};

}  // namespace shared::util
