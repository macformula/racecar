/// @author Blake Freer
/// @date 2024-03-21

#include "gtest/gtest.h"
#include "shared/util/mappers/constant.hpp"

namespace shared::util {

TEST(ConstantTest, Integer) {
    EXPECT_EQ(ConstantMap<int>::Evaluate(100, 4), 4);
}

TEST(ConstantTest, Float) {
    EXPECT_EQ(ConstantMap<float>::Evaluate(1.5e7f, -0.1f), -0.1f);
}

TEST(ConstantTest, FloatToInt) {
    using Cif = ConstantMap<int, float>;  // Template commas in EXPECT_ macro
                                          // confuse the preprocessor
    EXPECT_EQ(Cif::Evaluate(-2e9f, 1), 1);
}

}  // namespace shared::util