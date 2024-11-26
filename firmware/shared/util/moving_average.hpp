/// @author Blake Freer
/// @date 2023-12-02

#pragma once

#include <concepts>
#include <cstddef>

#include "shared/util/data_structures/circular_queue.hpp"

namespace shared::util {

template <typename T, size_t length>
    requires(std::is_arithmetic_v<T>) && (length > 0)
class MovingAverage {
public:
    MovingAverage() : sum_(0), count_(0) {}

    /// @brief Updates the moving average with a new value.
    void LoadValue(T new_value) {
        if (buffer_.is_full()) {
            sum_ -= buffer_.Dequeue();
        } else {
            count_ += 1;
        }
        sum_ += new_value;
        buffer_.Enqueue(new_value);
    }

    /// @brief Get the average of the last `length` values.
    T GetValue() {
        return T(sum_ / float(count_));
    }

private:
    CircularQueue<T, length> buffer_;
    float sum_;  // using float is not ideal, it should be an integer type for
                 // integer templates types, but using T does not guarantee a
                 // sufficient dynamic range.
    int count_;
};

}  // namespace shared::util
