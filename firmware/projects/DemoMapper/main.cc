/// @author Blake Freer
/// @date 2024-01-22

#include <iostream>  // for demo only, will not work on other platforms

#include "app.h"
#include "bindings.h"
#include "shared/util/mappers/clamper.h"
#include "shared/util/mappers/linear_map.h"
#include "shared/util/mappers/mapper.h"

using namespace shared::util;  // for demo only

LinearMap<float> double_plus_one{2, 1.0f};
Clamper<float> zero_to_ten{5, 10};

CompositeMap comp1{zero_to_ten, double_plus_one};
CompositeMap comp2{double_plus_one, comp1};

int main(void) {
    for (float i = 0; i < 7; i += 0.7) {
        float v1 = double_plus_one.Evaluate(i);
        float v2 = comp1.Evaluate(i);
        float v3 = comp2.Evaluate(i);

        std::cout << i << " (x2 + 1) " << v1 << " (clamp) " << v2
                  << " (x2 + 1) " << v3 << std::endl;
    }

    return 0;
}