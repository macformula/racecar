/// @author Blake Freer
/// @date 2024-03-21

#include "shared/util/mappers/lookup_table.h"

#include "gtest/gtest.h"

namespace shared::util {

// clang-format off
const float data[][2] = {
	{-11.f, 5.f},
	{ -1.f, 0.f},
	{  0.f, 3.f},
	{  5.f, 2.f}
};
// clang-format on

constexpr size_t data_rows = sizeof(data) / sizeof(data[0]);

LookupTable<data_rows> lut{data};

TEST(LookupTableTest, LowSideClamp) {
    EXPECT_FLOAT_EQ(lut.Evaluate(-12.f), 5.f);
}

TEST(LookupTableTest, HighSideClamp) {
    EXPECT_FLOAT_EQ(lut.Evaluate(6.f), 2.f);
}

TEST(LookupTableTest, KeyPoints) {
    // Lookup table should return exact value at specified points
    for (size_t i = 0; i < data_rows; i++) {
        EXPECT_FLOAT_EQ(lut.Evaluate(data[i][0]), data[i][1]);
    }
}

TEST(LookupTableTest, Interpolation) {
    EXPECT_FLOAT_EQ(lut.Evaluate(-4.f), 1.5f);
    EXPECT_FLOAT_EQ(lut.Evaluate(-0.7f), 0.9f);
    EXPECT_FLOAT_EQ(lut.Evaluate(4.f), 2.2f);
}

}  // namespace shared::util