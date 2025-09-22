/// @author Blake Freer
/// @date 2023-12-25

#include "bindings.hpp"

#include <iostream>

#include "mcal/cli/gpio.hpp"
#include "periph/gpio.hpp"

namespace mcal {
using namespace cli;

DigitalInput button_di{"Button"};
DigitalOutput indicator_do{"Indicator"};
}  // namespace mcal

namespace bindings {

macfe::periph::DigitalInput& button_di = mcal::button_di;
macfe::periph::DigitalOutput& indicator_do = mcal::indicator_do;

void Initialize() {
    std::cout << "Initializing CLI..." << std::endl;
}

}  // namespace bindings
