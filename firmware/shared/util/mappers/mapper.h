/// @author Blake Freer
/// @date 2024-01-22

#pragma once

#include <concepts>

namespace shared::util {

/**
 * @brief Evaulates a function.
 * @tparam T Output type.
 * @tparam U Input type, by default equal to `T`.
 * @note `T` and `U` must be numeric types.
 */
template <typename T, typename U = T>
    requires(std::is_arithmetic_v<T>)
class Mapper {
public:
    virtual T Evaluate(U x) const = 0;
};

/**
 * @brief Evaluates composed functions
 * @tparam Tf Output type of `f()`.
 * @tparam Tg Output type of `g()`.
 * @tparam U Input type to `g()`.
 */
template <typename Tf, typename Tg = Tf, typename U = Tg>
class CompositeMap : public Mapper<Tf, U> {
public:
    /**
     * @param f Outer function.
     * @param g Inner function.
     * @note Evaulates `f(g(x))`, so `g` is applied before `f`.
     */
    CompositeMap(Mapper<Tf, Tg>& f, Mapper<Tg, U>& g) : f_(f), g_(g) {}

    /**
     * @brief Evaluates `f(g(x))`.
     */
    static inline Tf Evaulate(Tg x, Mapper<Tf, Tg>& f, Mapper<Tg, U>& g) {
        return f.Evaluate(g.Evaluate(x));
    }

    inline Tf Evaluate(U x) const override {
        CompositeMap::Evaluate(x, f_, g_);
    }

private:
    const Mapper<Tf, Tg>& f_;
    const Mapper<Tg, U>& g_;
};

}  // namespace shared::util