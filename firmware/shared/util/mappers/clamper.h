/// @author Blake Freer
/// @date 2024-01-22

#ifndef SHARED_UTIL_MAPPERS_CLAMPER_H_
#define SHARED_UTIL_MAPPERS_CLAMPER_H_

#include "mapper.h"

namespace shared::util {

class Clamper : public Mapper {
public:
    Clamper(float min_val, float max_val)
        : min_val_(min_val), max_val_(max_val) {}

    float Evaluate(float key) override {
        if (key <= min_val_) {
            return min_val_;

        } else if (key >= max_val_) {
            return max_val_;

        } else {
            return key;
        }
    }

private:
    const float min_val_;
    const float max_val_;
};

}  // namespace shared::util

#endif