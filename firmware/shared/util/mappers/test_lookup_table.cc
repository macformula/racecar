/// @author Blake Freer
/// @date 2024-03-21

#include "gtest/gtest.h"
#include "shared/util/mappers/lookup_table.hpp"

namespace shared::util {

auto data = std::to_array<LookupTable<float>::Entry>({
    {-11.f, 5.f},
    {-1.f, 0.f},
    {0.f, 3.f},
    {5.f, 2.f},
});

LookupTable<float> lut{data};

TEST(LookupTableTest, LowSideClamp) {
    EXPECT_FLOAT_EQ(lut.Evaluate(-12.f), 5.f);
}

TEST(LookupTableTest, HighSideClamp) {
    EXPECT_FLOAT_EQ(lut.Evaluate(6.f), 2.f);
}

TEST(LookupTableTest, KeyPoints) {
    // Lookup table should return exact value at specified points
    for (const auto& entry : data) {
        EXPECT_FLOAT_EQ(lut.Evaluate(entry.key), entry.value);
    }
}

TEST(LookupTableTest, Interpolation) {
    EXPECT_FLOAT_EQ(lut.Evaluate(-4.f), 1.5f);
    EXPECT_FLOAT_EQ(lut.Evaluate(-0.7f), 0.9f);
    EXPECT_FLOAT_EQ(lut.Evaluate(4.f), 2.2f);
}

}  // namespace shared::util