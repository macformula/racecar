/// @author Blake Freer
/// @date 2023-12-02

#pragma once

#include <concepts>

#include "shared/util/data_structures/circular_queue.h"

namespace shared::util {

template <typename T, size_t length>
    requires(std::is_arithmetic_v<T>) && (length > 0)
class MovingAverage {
public:
    MovingAverage() : sum_(0), count_(0), moving_average_value_(0) {}

    /// @brief Updates the moving average with a new value.
    void LoadValue(T new_value) {
        if (buffer_.is_full()) {
            sum_ -= buffer_.Dequeue();
        } else {
            count_ += 1;
        }
        sum_ += new_value;
        buffer_.Enqueue(new_value);

        moving_average_value_ = sum_ / (float)count_;
    }

    /// @brief Get the average of the last `length` values.
    T GetValue() {
        return moving_average_value_;
    }

private:
    CircularQueue<T, length> buffer_;
    T sum_;
    int count_;
    T moving_average_value_;
};

}  // namespace shared::util
