/// @author Blake Freer
/// @date 2024-04-21

#pragma once

#include "shared/util/mappers/mapper.hpp"

namespace shared::util {

/**
 * @brief The most basic map; that which returns its input. This is added for
 * completeness when a mapper object is required but has no behaviour.
 *
 * @tparam T Output Type
 * @tparam U Input Type, default = `T`
 */
template <typename T, typename U = T>
class IdentityMap : public shared::util::Mapper<T, U> {
public:
    IdentityMap() {}

    inline T Evaluate(U x) const override {
        return T(x);
    }

    // There is no static Evaluate() since this class should only be used as a
    // placeholder object, not on its own.
};

}  // namespace shared::util