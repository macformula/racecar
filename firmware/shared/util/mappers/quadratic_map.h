/// @author Blake Freer
/// @date 2024-01-22

#ifndef SHARED_UTIL_MAPPERS_QUADRATIC_MAP_
#define SHARED_UTIL_MAPPERS_QUADRATIC_MAP_

namespace shared::util {

class QuadraticMap {
public:
    QuadraticMap(float a, float b, float c) : a_(a), b_(b), c_(c) {}

    float Evaluate(float key) {
        return a_ * key * key + b_ * key + c_;
    }

private:
    float a_;
    float b_;
    float c_;
};

}  // namespace shared::util

#endif