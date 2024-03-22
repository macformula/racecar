/// @author Blake Freer
/// @date 2024-03-21

#include "shared/util/mappers/clamper.h"

#include <limits.h>

#include "gtest/gtest.h"

namespace shared::util {

TEST(ClamperTest, ClampsExternal) {
    EXPECT_EQ(Clamper<int>::Evaluate(-5, 1, 10), 1);
    EXPECT_EQ(Clamper<int>::Evaluate(100, 1, 10), 10);

    EXPECT_FLOAT_EQ(Clamper<float>::Evaluate(-1e6f, -100.f, 100.f), -100.f);
    EXPECT_FLOAT_EQ(Clamper<float>::Evaluate(0.0001f, 0.0002f, 1.0f), 0.0002f);
}

TEST(ClamperTest, LeavesInternal) {
    EXPECT_EQ(Clamper<int>::Evaluate(3, 0, 5), 3);
    EXPECT_EQ(Clamper<int>::Evaluate(0, 0, 5), 0);
    EXPECT_EQ(Clamper<int>::Evaluate(5, 0, 5), 5);

    EXPECT_FLOAT_EQ(Clamper<float>::Evaluate(5.6f, 0.0f, 10.0f), 5.6f);
    EXPECT_FLOAT_EQ(Clamper<float>::Evaluate(0.0001f, 0.0f, 5.0f), 0.0001f);
    EXPECT_FLOAT_EQ(Clamper<float>::Evaluate(4.9999f, 0.0f, 5.0f), 4.9999f);
}

TEST(ClamperTest, OneSided) {
    EXPECT_EQ(Clamper<int>::Evaluate(-10, 0, std::numeric_limits<int>().max()),
              0);
    EXPECT_EQ(Clamper<int>::Evaluate(230, 0, std::numeric_limits<int>().max()),
              230);

    EXPECT_FLOAT_EQ(
        Clamper<float>::Evaluate(1e9, 0, std::numeric_limits<float>().max()),
        1e9f);
    EXPECT_FLOAT_EQ(Clamper<float>::Evaluate(
                        1e9f, std::numeric_limits<float>().min(), 0.1f),
                    0.1f);
}

TEST(ClamperTest, DifferentDomains) {
    using Cfi = Clamper<float, int>;  // Template commas in EXPECT_ macro
                                      // confuse the preprocessor

    EXPECT_EQ(Cfi::Evaluate(3, -2.5f, 2.5f), 2.5f);
    EXPECT_EQ(Cfi::Evaluate(2, -2.5f, 2.5f), 2.0f);

    EXPECT_EQ(Cfi::Evaluate(-2, -2.5f, 2.5f), -2.0f);
    EXPECT_EQ(Cfi::Evaluate(-3, -2.5f, 2.5f), -2.5f);

    using Cif = Clamper<int, float>;
    EXPECT_EQ(Cif::Evaluate(2.5f, -2, 2), 2);
    EXPECT_EQ(Cif::Evaluate(1.1f, -2, 2), 1);
    EXPECT_EQ(Cif::Evaluate(1.99f, -2, 2), 1);
    EXPECT_EQ(Cif::Evaluate(-1.99f, -2, 2), -1);
    EXPECT_EQ(Cif::Evaluate(-2.001f, -2, 2), -2);
}

}  // namespace shared::util