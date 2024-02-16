/// @author Blake Freer
/// @date 2024-02-10

#pragma once

#include "mapper.h"

namespace shared::util {

/**
 * @brief Evaluates the constant function `f(x) = c`.
 * @tparam T Output type.
 * @tparam U Input type.
 * @note Always evaluates to the same value. Included for completeness.
 */
template <typename T, typename U = T>
class ConstantMap : public Mapper<T, T> {
public:
    ConstantMap(T c) : c_(c) {}

    static inline T Evaluate(U x, T c) {
        return c;
    }

    inline T Evaluate(U x) const override {
        return c_;
    }

private:
    const T c_;
};

}  // namespace shared::util