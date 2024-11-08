#include <unistd.h>

#include "bindings.h"
#include "mcal/linux/periph/digital_output.h"
#include "shared/periph/gpio.h"

namespace mcal {

using namespace lnx::periph;

DigitalOutput indicator{"Indicator"};

}  // namespace mcal

namespace bindings {

shared::periph::DigitalOutput& indicator = mcal::indicator;

void DelayMS(unsigned int ms) {
    usleep(1000 * ms);
}

void Initialize() {
    std::cout << "Initializing Linux" << std::endl;
}

}  // namespace bindings