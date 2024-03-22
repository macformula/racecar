/// @author Blake Freer
/// @date 2024-03-21

#include "shared/util/mappers/linear_map.h"

#include "gtest/gtest.h"

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
}

}  // namespace shared::util