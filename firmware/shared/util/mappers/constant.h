/// @author Blake Freer
/// @date 2024-02-10

#pragma once

#include "mapper.h"

namespace shared::util {

class ConstantMap : public Mapper {
public:
    ConstantMap(float c) : c_(c) {}

    float Evaluate(float key) const override {
        return c_;
    }

private:
    const float c_;
};

}  // namespace shared::util