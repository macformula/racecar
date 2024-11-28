/// @author Blake Freer
/// @date 2024-03-21

#include "gtest/gtest.h"
#include "shared/util/mappers/linear_map.hpp"

namespace shared::util {

TEST(LinearMapTest, NoSlope) {
    EXPECT_FLOAT_EQ(LinearMap<float>::Evaluate(1.0f, 0.0f, 10.0f), 10.0f);
    EXPECT_EQ(LinearMap<int>::Evaluate(-10, 0, -7), -7);

    using LMif = LinearMap<int, float>;
    EXPECT_EQ(LMif::Evaluate(8.2f, 0, 2), 2);

    using LMfi = LinearMap<float, int>;
    EXPECT_FLOAT_EQ(LMfi::Evaluate(10, 0.0f, 2.5f), 2.5f);
}

TEST(LinearMapTest, FractionSlope) {
    using LMfi = LinearMap<float, int>;
    EXPECT_FLOAT_EQ(LMfi::Evaluate(5, 0.5f, 1.0f), 3.5f);

    // 0.9 should be converted to int 0 before any operations are done
    EXPECT_EQ(LinearMap<int>::Evaluate(10, 0.9, 0), 0);

    using LMif = LinearMap<int, float>;
    EXPECT_EQ(LMif::Evaluate(10.0f, 0.9, 0), 0);

    EXPECT_FLOAT_EQ(LinearMap<float>::Evaluate(10.0f, 0.9f, 0.0f), 9.0f);
}

TEST(LinearMapTest, Integer) {
    using LMi = LinearMap<int>;
    EXPECT_EQ(LMi::Evaluate(3, 2, -7), -1);
    EXPECT_EQ(LMi::Evaluate(4, 2, -7), 1);
    EXPECT_EQ(LMi::Evaluate(-100, -2, 1), 201);
}

TEST(LinearMapTest, Float) {
    using LMf = LinearMap<float>;
    EXPECT_FLOAT_EQ(LMf::Evaluate(0.5f, -0.5f, 1.75f), 1.5f);
    EXPECT_FLOAT_EQ(LMf::Evaluate(1e4f, 1e3f, 2e5f), 1.02e7f);
    EXPECT_FLOAT_EQ(LMf::Evaluate(1e4f, 1e-4f, 7.0f), 8.0f);
}

TEST(LinearMaptest, Overflow) {
    using LMu8 = LinearMap<uint8_t>;
    EXPECT_EQ(LMu8::Evaluate(10, 25, 10), 4);
    EXPECT_EQ(LMu8::Evaluate(20, -2, 50), 10);
}

}  // namespace shared::util