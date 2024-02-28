/// @author Blake Freer
/// @date 2023-12-25

#include <iostream>

#include "mcal/windows/periph/adc.h"
#include "mcal/windows/periph/gpio.h"
#include "shared/periph/adc.h"
#include "shared/periph/gpio.h"

namespace mcal {}  // namespace mcal

namespace bindings {

void Initialize() {
    std::cout << "Initializing Windows..." << std::endl;
}

}  // namespace bindings
