/// @author Blake Freer
/// @date 2023-10-14
/// @note Taken from Christopher Kormanyos

#ifndef SHARED_UTIL_NONCOPYABLE_H_
#define SHARED_UTIL_NONCOPYABLE_H_

namespace shared::util {

/// @brief Usage: Privately inherit `util::Noncopyable` to prevent copy
/// constructors and reassignment.
class Noncopyable {
protected:
    Noncopyable() = default;
    ~Noncopyable() = default;

private:
    Noncopyable(const Noncopyable&) = delete;
    Noncopyable(Noncopyable&&) = delete;

    auto operator=(const Noncopyable&) -> Noncopyable& = delete;
    auto operator=(Noncopyable&&) -> Noncopyable& = delete;
};

}  // namespace shared::util

#endif