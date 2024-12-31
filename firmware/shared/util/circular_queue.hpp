/// @author Blake Freer
/// @date 2023-12-02

#pragma once

#include <cstddef>

namespace shared::util {

template <typename T, size_t length>
    requires(length > 0)
class CircularQueue {
public:
    CircularQueue() : head_(0), tail_(0), full_flag_(false) {}

    /// @brief Adds an item to the back of the queue.
    /// @param item
    /// @note If the queue is full, no action is performed. Check `.is_full()`
    /// before enqueuing
    void Enqueue(T item) {
        if (full_flag_) {
            return;
        }
        buffer_[head_] = item;
        head_ = (head_ + 1) % length;

        empty_flag_ = false;
        full_flag_ = (head_ == tail_);
    }

    /// @brief Pop and return the item at the front of the queue.
    /// @return The front item.
    /// @note If the queue is empty, `0` is returned. Check `.is_empty()` before
    /// dequeuing.
    T Dequeue() {
        if (tail_ == head_ && !full_flag_) {
            return T{};
        }

        T item = buffer_[tail_];
        tail_ = (tail_ + 1) % length;

        full_flag_ = false;
        empty_flag_ = (head_ == tail_);

        return item;
    }

    /// @brief Returns `true` if the queue is full.
    bool is_full() const {
        return full_flag_;
    }

    /// @brief Returns `true` is the queue is empty.
    bool is_empty() const {
        return empty_flag_;
    }

private:
    T buffer_[length];
    size_t head_;
    size_t tail_;
    bool full_flag_;
    bool empty_flag_;
};

}  // namespace shared::util
