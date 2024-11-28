/// @author Blake Freer
/// @date 2024-03-21

#include "gtest/gtest.h"
#include "shared/util/mappers/quadratic_map.hpp"

namespace shared::util {

TEST(QuadraticMapTest, Float) {
    using QMf = QuadraticMap<float>;

    EXPECT_FLOAT_EQ(QMf::Evaluate(2.5f, -4.9f, 1.0f, 10.0f), -18.125);
    EXPECT_FLOAT_EQ(QMf::Evaluate(10.5f, 0.0f, 3.0f, 1.0f), 32.5f);
    EXPECT_FLOAT_EQ(QMf::Evaluate(2.5f, 1.0f, -2.0f, 1.0f), 2.25f);
}

TEST(QuadraticMapTest, Integer) {
    using QMi = QuadraticMap<int>;

    EXPECT_EQ(QMi::Evaluate(3, 2, -1, -4), 11);      // 2x^2 - x - 4
    EXPECT_EQ(QMi::Evaluate(4, 1, 0, 0), 16);        // x^2
    EXPECT_EQ(QMi::Evaluate(-100, 1, 0, 0), 10000);  // x^2
}

}  // namespace shared::util