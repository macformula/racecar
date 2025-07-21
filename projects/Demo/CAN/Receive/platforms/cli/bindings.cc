/// @author Samuel Parent
/// @date 2024-01-16

#include "../../bindings.hpp"

#include <iostream>

#include "lib/periph/can.hpp"
#include "mcal/cli/can.hpp"
#include "mcal/cli/gpio.hpp"

namespace mcal {
using namespace cli;

CanBase demo_can_base{"vcan0"};
DigitalOutput indicator{"Indicator"};
}  // namespace mcal

namespace bindings {
macfe::periph::CanBase& demo_can_base = mcal::demo_can_base;
macfe::periph::DigitalOutput& indicator = mcal::indicator;

void Initialize() {
    std::cout << "Initializing CLI..." << std::endl;
    mcal::demo_can_base.Setup();
    std::cerr << "The CLI platform has no way to receive messages from "
                 "CAN/Send. Consider using the linux platform instead."
              << std::endl;
    std::cerr << "Exiting..." << std::endl;
    exit(1);
}
}  // namespace bindings