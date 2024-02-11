/// @author Blake Freer
/// @date 2024-01-22

#ifndef SHARED_UTIL_MAPPERS_QUADRATIC_MAP_
#define SHARED_UTIL_MAPPERS_QUADRATIC_MAP_

#include "mapper.h"

namespace shared::util {

class QuadraticMap : public Mapper {
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

#endif