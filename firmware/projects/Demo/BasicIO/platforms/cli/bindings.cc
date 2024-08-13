/// @author Blake Freer
/// @date 2023-12-25

#include <iostream>

#include "mcal/cli/periph/gpio.h"
#include "shared/periph/gpio.h"

namespace mcal {
using namespace cli::periph;

DigitalInput button_di{"Button"};
DigitalOutput indicator_do{"Indicator"};
}  // namespace mcal

namespace bindings {

const shared::periph::DigitalInput& button_di = mcal::button_di;
const shared::periph::DigitalOutput& indicator_do = mcal::indicator_do;

void Initialize() {
    std::cout << "Initializing CLI..." << std::endl;
}

}  // namespace bindings
