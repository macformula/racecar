/// @author Blake Freer
/// @date 2024-03-21
/// @details Tests the order of operations of CompositeMap.

#include "gtest/gtest.h"
#include "shared/util/mappers/clamper.hpp"
#include "shared/util/mappers/linear_map.hpp"
#include "shared/util/mappers/mapper.hpp"

namespace shared::util {

LinearMap<float> linear{2.f, -5.f};
Clamper<float> clamp{-8.f, 2.f};

using CompF = CompositeMap<float>;

TEST(CompositeMapTest, LinearThenClamp) {
    CompositeMap<float> lin_then_clamp{linear, clamp};

    EXPECT_EQ(lin_then_clamp.Evaluate(1.f), -3.f);
    EXPECT_EQ(lin_then_clamp.Evaluate(5.f), 2.f);
    EXPECT_EQ(lin_then_clamp.Evaluate(-1.f), -7.f);
    EXPECT_EQ(lin_then_clamp.Evaluate(-2.f), -8.f);
}

TEST(CompositeMapTest, ClampThenLinear) {
    CompositeMap<float> clamp_then_lin{clamp, linear};

    EXPECT_EQ(clamp_then_lin.Evaluate(1.f), -3.f);
    EXPECT_EQ(clamp_then_lin.Evaluate(3.f), -1.f);
    EXPECT_EQ(clamp_then_lin.Evaluate(100.f), -1.f);
    EXPECT_EQ(clamp_then_lin.Evaluate(-7.f), -19.f);
    EXPECT_EQ(clamp_then_lin.Evaluate(-8.f), -21.f);
    EXPECT_EQ(clamp_then_lin.Evaluate(-18.f), -21.f);
}
}  // namespace shared::util