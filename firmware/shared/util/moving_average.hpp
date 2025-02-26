/// @author Blake Freer
/// @date 2023-12-02

#pragma once

#include <cstddef>
#include <type_traits>

#include "etl/circular_buffer.h"

namespace shared::util {

template <size_t length, typename T = float>
    requires(std::is_floating_point_v<T>) && (length > 0)
class MovingAverage {
public:
    MovingAverage() : sum_(0) {
        // preload with zeros
        for (size_t i = 0; i < length; i++) {
            buffer_.push(0);
        }
    }

    /// @brief Updates the moving average with a new value.
    void LoadValue(T new_value) {
        sum_ -= buffer_.front();  // safe since buffer is preloaded with 0
        sum_ += new_value;
        buffer_.push(new_value);
    }

    /// @brief Get the average of the last `length` values.
    T GetValue() {
        return sum_ / static_cast<T>(length);
    }

private:
    etl::circular_buffer<T, length> buffer_;
    float sum_;
};

}  // namespace shared::util
