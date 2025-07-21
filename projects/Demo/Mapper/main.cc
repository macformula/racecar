/// @author Blake Freer
/// @date 2024-12-28

#include <format>
#include <iostream>  // for demo only, will not work on other platforms

#include "lib/util/lookup_table.hpp"

void expect_eq(double actual, double expected) {
    std::cout << std::format("Expected {:.2f}, got: {:.2f}", expected, actual);
    if (std::abs(expected - actual) > 0.01) {
        std::cout << "  FAIL";
    }
    std::cout << std::endl;
}

int main(void) {
    using LUT = macfe::util::LookupTable<float>;

    std::cout << "Lookup Table" << std::endl;
    auto data = std::to_array<LUT::Entry>({
        {-2., 4.},
        {-1., 1.},
        {0., 0.},
        {1., 1.},
        {2., 4.},
        {3., 9.},
    });

    expect_eq(LUT::Evaluate(data, -1.5), 2.5);
    expect_eq(LUT::Evaluate(data, 0.5), 0.5);
    expect_eq(LUT::Evaluate(data, 2.), 4);
    expect_eq(LUT::Evaluate(data, 4.), 9);   // (above highest key)
    expect_eq(LUT::Evaluate(data, -4.), 4);  // (below lowest key)

    return 0;
}