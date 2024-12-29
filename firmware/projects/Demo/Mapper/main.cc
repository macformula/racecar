/// @author Blake Freer
/// @date 2024-01-22

#include <format>
#include <iostream>  // for demo only, will not work on other platforms

#include "shared/util/mappers/clamper.hpp"
#include "shared/util/mappers/linear_map.hpp"
#include "shared/util/mappers/lookup_table.hpp"
#include "shared/util/mappers/mapper.hpp"

using namespace shared::util;  // for demo only

LinearMap<float> double_plus_one{2, 1.0f};
Clamper<float> zero_to_ten{5, 10};

CompositeMap comp1{zero_to_ten, double_plus_one};
CompositeMap comp2{double_plus_one, comp1};

// Lookup table example
const float lut_data[][2]{
    {0.f, 0.f},
    {5.f, 2.f},
    {8.f, 4.f},
};
constexpr int lut_length = (sizeof(lut_data)) / (sizeof(lut_data[0]));
LookupTable<lut_length, float> lut{lut_data};

int main(void) {
    for (float i = 0; i < 7; i += 0.7) {
        std::cout
            << std::format(
                   "{:.1f} (x2 + 1) {:.1f} (clamp) {:.1f} (x2 + 1) {:.1f}", i,
                   double_plus_one.Evaluate(i), comp1.Evaluate(i),
                   comp2.Evaluate(i))
            << std::endl;
    }

    for (float f = 0; f <= 10; f += 1.f) {
        std::cout << std::format("{:.2f} -> LUT -> {:.2f}", f, lut.Evaluate(f))
                  << std::endl;
    }
    return 0;
}