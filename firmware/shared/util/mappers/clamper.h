/// @author Blake Freer
/// @date 2024-01-22

#pragma once

#include "mapper.h"

namespace shared::util {

/**
 * @brief Clamps a value between an `upper` and `lower` limit.
 * @tparam T Output type.
 * @tparam U Input type.
 * @note To only clamp from above, pass `std::numeric_limits<T>().minimum()` as
 * `lower`, and `.maximum()` as `upper` for a clamp from below.
 */
template <typename T, typename U = T>
class Clamper : public Mapper<T, U> {
public:
    Clamper(T lower, T upper) : lower_(lower), upper_(upper) {}

    inline static T Evaluate(U value, T lower, T upper) {
        if (value <= lower) {
            return lower;
        } else if (value >= upper) {
            return upper;
        } else {
            return value;
        }
    }

    inline T Evaluate(U x) const override {
        return Clamper<T, U>::Evaluate(x, lower_, upper_);
    }

private:
    const float lower_;
    const float upper_;
};

}  // namespace shared::util
