/// @author Blake Freer
/// @date 2024-01-22

#pragma once

#include <type_traits>
namespace shared::util {

/// @brief Evaluates a function.
/// @tparam T Output type, by default `float`.
/// @tparam U Input type, by default equal to `T`.
/// @note `T` and `U` must be numeric types.
template <typename T, typename U = T>
    requires(std::is_arithmetic_v<T>)
class Mapper {
public:
    virtual T Evaluate(U x) const = 0;
};

/// @brief Evaluates composed functions
/// @tparam Tf Output type of `f()`.
/// @tparam Tg Output type of `g()`.
/// @tparam U Input type to `g()`.
template <typename Tf, typename Tg = Tf, typename U = Tg>
class CompositeMap : public Mapper<Tf, U> {
public:
    /// @param g Inner function.
    /// @param f Outer function.
    /// @note Evaulates `f(g(x))`, so `g` is applied before `f`.
    CompositeMap(const Mapper<Tg, U>& g, const Mapper<Tf, Tg>& f)
        : g_(g), f_(f) {}

    inline Tf Evaluate(U x) const override {
        return f_.Evaluate(g_.Evaluate(x));
    }

    // No static `Evaluate()` since you should directly compose the functions.

private:
    const Mapper<Tg, U>& g_;
    const Mapper<Tf, Tg>& f_;
};

}  // namespace shared::util