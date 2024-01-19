/// @author Blake Freer
/// @date 2023-12-02

#ifndef SHARED_DATA_STRUCTURES_CIRCULAR_QUEUE_H_
#define SHARED_DATA_STRUCTURES_CIRCULAR_QUEUE_H_

#include <concepts>
#include <cstdint>
#include <stdexcept>

namespace shared::util {

class QueueFullException : public std::exception {
public:
    const char* what() const throw() {
        return "Queue is full.";
    }
};

class QueueEmptyException : public std::exception {
public:
    const char* what() const throw() {
        return "Queue is empty.";
    }
};

template <typename T, uint32_t length>
class CircularQueue {
public:
    CircularQueue() : head_(0), tail_(0), full_flag(false) {}

    void Enqueue(T item) {
        if (full_flag) {
            throw QueueFullException();
        }
        buffer_[head_] = item;
        head_ = (head_ + 1) % length;

        full_flag = (head_ == tail_);
    }

    T Dequeue() {
        if (tail_ == head_ && !full_flag) {
            throw QueueEmptyException();
        }

        T item = buffer_[tail_];
        tail_ = (tail_ + 1) % length;
        full_flag = false;
        return item;
    }

private:
    T buffer_[length];
    int tail_;
    int head_;
    bool full_flag;
};

}  // namespace shared::util

#endif