/// @author Blake Freer
/// @date 2024-03-21

#include "gtest/gtest.h"
#include "shared/util/mappers/constant.hpp"

namespace shared::util {

TEST(ConstantTest, Integer) {
    EXPECT_EQ(ConstantMap<int>(4).Evaluate(100), 4);
}

TEST(ConstantTest, Float) {
    EXPECT_EQ(ConstantMap<float>(-0.1f).Evaluate(1.5e7f), -0.1f);
}

TEST(ConstantTest, FloatToInt) {
    using Cif = ConstantMap<int, float>;  // Template commas in EXPECT_ macro
                                          // confuse the preprocessor
    EXPECT_EQ(Cif(1).Evaluate(-2e9), 1);
}

}  // namespace shared::util