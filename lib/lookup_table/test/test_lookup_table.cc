/// @author Blake Freer
/// @date 2024-03-21

#include "gtest/gtest.h"
#include "lookup_table.hpp"

using macfe::LookupTable;

constexpr auto data = std::to_array<LookupTable::Entry>({
    {-11.f, 5.f},
    {-1.f, 0.f},
    {0.f, 3.f},
    {5.f, 2.f},
});

TEST(LookupTable, LowSideClamp) {
    EXPECT_FLOAT_EQ(LookupTable::Evaluate(data, -12.f), 5.f);
}

TEST(LookupTable, HighSideClamp) {
    EXPECT_FLOAT_EQ(LookupTable::Evaluate(data, 6.f), 2.f);
}

TEST(LookupTable, KeyPoints) {
    // Lookup table should return exact value at specified points
    for (const auto& entry : data) {
        EXPECT_FLOAT_EQ(LookupTable::Evaluate(data, entry.key), entry.value);
    }
}

TEST(LookupTable, Interpolation) {
    EXPECT_FLOAT_EQ(LookupTable::Evaluate(data, -4.f), 1.5f);
    EXPECT_FLOAT_EQ(LookupTable::Evaluate(data, -0.7f), 0.9f);
    EXPECT_FLOAT_EQ(LookupTable::Evaluate(data, 4.f), 2.2f);
}

int main(int argc, char** argv) {
    ::testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}