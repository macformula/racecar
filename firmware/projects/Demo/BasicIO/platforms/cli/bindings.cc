/// @author Blake Freer
/// @date 2023-12-25

#include "../../bindings.hpp"

#include <iostream>

#include "mcal/cli/periph/gpio.hpp"
#include "shared/periph/gpio.hpp"

namespace mcal {
using namespace cli::periph;

DigitalInput button_di{"Button"};
DigitalOutput indicator_do{"Indicator"};
}  // namespace mcal

namespace bindings {

shared::periph::DigitalInput& button_di = mcal::button_di;
shared::periph::DigitalOutput& indicator_do = mcal::indicator_do;

void Initialize() {
    std::cout << "Initializing CLI..." << std::endl;
}

}  // namespace bindings
