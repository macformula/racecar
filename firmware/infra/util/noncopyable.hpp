/// @author Blake Freer
/// @date 2023-10-14
/// @note Taken from Christopher Kormanyos

#ifndef UTIL_NONCOPYABLE_HPP
#define UTIL_NONCOPYABLE_HPP

namespace util {

/// @brief Privately inherit `util::noncopyable` to prevent copy constructors
/// and reassignment.
class noncopyable {
protected:
    noncopyable() = default;
    ~noncopyable() = default;

private:
    noncopyable(const noncopyable&) = delete;
    noncopyable(noncopyable&&) = delete;

    auto operator=(const noncopyable&) -> noncopyable& = delete;
    auto operator=(noncopyable&&) -> noncopyable& = delete;
};

}  // namespace util

#endif