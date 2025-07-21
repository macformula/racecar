/// @author Blake Freer
/// @date 2023-12-02

#pragma once

#include <cstddef>

#include "etl/circular_buffer.h"

namespace shared::util {

template <size_t length>
class MovingAverage {
public:
    MovingAverage() : sum_(0) {
        // preload with zeros
        for (size_t i = 0; i < length; i++) {
            buffer_.push(0);
        }
    }

    /// @brief Updates the moving average with a new value.
    void LoadValue(float new_value) {
        sum_ -= buffer_.front();  // safe since buffer is preloaded with 0
        sum_ += new_value;
        buffer_.push(new_value);
    }

    /// @brief Get the average of the last `length` values.
    float GetValue() {
        return sum_ * INV_LENGTH;
    }

private:
    etl::circular_buffer<float, length> buffer_;
    float sum_;
    const float INV_LENGTH = 1.f / length;
};

}  // namespace shared::util
