/// @author Samuel Parent
/// @date 2024-01-16

#include <chrono>
#include <iostream>
#include <thread>

#include "../../bindings.h"
#include "mcal/cli/periph/can.h"
#include "mcal/cli/periph/gpio.h"
#include "shared/periph/can.h"

namespace mcal {
using namespace cli::periph;

CanBase veh_can_base{"vcan0"};
DigitalOutput indicator{"Indicator"};
}  // namespace mcal

namespace bindings {
shared::periph::CanBase& veh_can_base = mcal::veh_can_base;
shared::periph::DigitalOutput& indicator = mcal::indicator;

void Initialize() {
    std::cout << "Initializing CLI..." << std::endl;
    mcal::veh_can_base.Setup();
    std::cerr << "The CLI platform has no way to receive messages from "
                 "CAN/Send. Consider using the linux platform instead."
              << std::endl;
    std::cerr << "Exiting..." << std::endl;
    exit(1);
}
}  // namespace bindings