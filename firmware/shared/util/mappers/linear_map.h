/// @author Blake Freer
/// @date 2024-01-22

#pragma once

#include "mapper.h"

namespace shared::util {

/**
 * @brief Evaluates the linear function `f(x) = mx + b`.
 * @tparam T Ouput type.
 * @tparam U Input type.
 */
template <typename T, typename U = T>
class LinearMap : public Mapper<T, U> {
public:
    LinearMap(T m, T b) : m_(m), b_(b) {}

    static inline T Evaluate(U x, T m, T b) {
        return m * x + b;
    }

    inline T Evaluate(U x) const override {
        return LinearMap<T, U>::Evaluate(x, m_, b_);
    }

private:
    const T m_;
    const T b_;
};

}  // namespace shared::util
