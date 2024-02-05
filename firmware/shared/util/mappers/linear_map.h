/// @author Blake Freer
/// @date 2024-01-22

#ifndef SHARED_UTIL_MAPPERS_LINEAR_MAP_H_
#define SHARED_UTIL_MAPPERS_LINEAR_MAP_H_

#include "mapper.h"

namespace shared::util {

class LinearMap : public Mapper {
public:
    LinearMap(float m, float b) : m_(m), b_(b) {}

    float Evaluate(float key) const override {
        return key * m_ + b_;
    }

private:
    float const m_;
    float const b_;
};

}  // namespace shared::util

#endif