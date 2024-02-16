/// @author Blake Freer
/// @date 2024-01-22

#pragma once

#include "mapper.h"

namespace shared::util {
/**
 * @brief Evaluates a quadratic function `f(x) = axx + bx + c`.
 * @tparam T Ouptut type.
 * @tparam U Input type.
 */
template <typename T, typename U = T>
class QuadraticMap : public Mapper<T, U> {
public:
    QuadraticMap(float a, float b, float c) : a_(a), b_(b), c_(c) {}

    float Evaluate(float key) const override {
        return a_ * key * key + b_ * key + c_;
    }

private:
    const float a_;
    const float b_;
    const float c_;
};

}  // namespace shared::util
