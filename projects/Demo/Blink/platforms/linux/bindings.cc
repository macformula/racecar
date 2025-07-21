#include "../../bindings.hpp"

#include <unistd.h>

#include "lib/periph/gpio.hpp"
#include "mcal/linux/digital_output.hpp"

namespace mcal {

using namespace lnx;

DigitalOutput indicator{"Indicator"};

}  // namespace mcal

namespace bindings {

macfe::periph::DigitalOutput& indicator = mcal::indicator;

void DelayMS(unsigned int ms) {
    usleep(1000 * ms);
}

void Initialize() {
    std::cout << "Initializing Linux" << std::endl;
}

}  // namespace bindings