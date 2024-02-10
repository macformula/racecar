/// @author Blake Freer
/// @date 2023-12-02

#ifndef SHARED_UTIL_MOVING_AVERAGE_H_
#define SHARED_UTIL_MOVING_AVERAGE_H_

#include <concepts>

#include "shared/util/data_structures/circular_queue.h"

namespace shared::util {

template <typename T, int length>
    requires(std::is_arithmetic_v<T>) && (length > 0)
class MovingAverage {
public:
    MovingAverage() : sum_(0), count_(0) : moving_average_value_(0) {}

    /**
     * @brief Updates the moving average with a new value.
     */
    void LoadValue(T new_value) {
        sum_ += new_value;
        try {
            buffer_.Enqueue(new_value);
            count_ += 1;  // Not reached if Enqueue throws Full exception
        } catch (QueueFullException& e) {
            sum_ -= buffer_.Dequeue();
            buffer_.Enqueue(new_value);
            // Count is not modified since one element was added and removed
        }
        moving_average_value_ = sum_ / count_;
    }

    /**
     * @brief Get the average of the last `length` values.
     */
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

#endif