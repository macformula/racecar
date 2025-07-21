// Blake Freer
// November 2024

#pragma once

#include <atomic>
#include <optional>

namespace macfe {

/// @brief Thread & interrupt safe SPSC buffer.
/// `Get` and `Pop` return an `std::optional` since the buffer may be empty.
/// Calling `Pop` will clear the buffer.
///
/// Assumes that `Get` and `Pop` may be interrupted by `Add`, but `Add` will not
/// be interrupted by `Get` or `Pop`. Intended for use where the producer
/// calling `Add` is an interrupt.
///
/// @tparam T Value type to hold
/// @warning Data race will occur if a single `Pop` call is interrupted twice
/// by `Add` since `Pop` will clear the second written message.
template <typename T>
class AtomicBuffer {
public:
    AtomicBuffer() : read_pos_(0) {
        buffer_[0] = std::nullopt;
        buffer_[1] = std::nullopt;
    }

    /// Get the latest item
    std::optional<T> Get();

    /// Get the latest item and clear the buffer
    std::optional<T> Pop();

    /// Add an item to the buffer
    void Add(const T& new_item);

    /// Move an rvalue item into the buffer
    void Add(T&& new_item);

private:
    std::atomic<int> read_pos_;
    std::optional<T> buffer_[2];
};

template <typename T>
std::optional<T> AtomicBuffer<T>::Get() {
    int read_pos = read_pos_.load(std::memory_order_acquire);
    return buffer_[read_pos];
}

template <typename T>
std::optional<T> AtomicBuffer<T>::Pop() {
    int read_pos = read_pos_.load(std::memory_order_acquire);
    auto val = std::move(buffer_[read_pos]);
    buffer_[read_pos] = std::nullopt;
    return val;
}

template <typename T>
void AtomicBuffer<T>::Add(const T& new_item) {
    // write to the "other" side of the buffer
    int write_pos = 1 - read_pos_.load(std::memory_order_relaxed);
    buffer_[write_pos] = new_item;
    read_pos_.store(write_pos, std::memory_order_release);
}

template <typename T>
void AtomicBuffer<T>::Add(T&& new_item) {
    int write_pos = 1 - read_pos_.load(std::memory_order_relaxed);
    buffer_[write_pos] = std::move(new_item);
    read_pos_.store(write_pos, std::memory_order_release);
}

}  // namespace macfe