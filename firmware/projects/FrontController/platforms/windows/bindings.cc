/// @author Blake Freer
/// @date 2023-12-25

#include "bindings.h"

#include <iostream>

namespace bindings {

mcal::periph::DigitalInput button_di{"Button"};
mcal::periph::DigitalOutput indicator_do{"Indicator"};

void Initialize() {
    std::cout << "Initializing Windows..." << std::endl;
}

}  // namespace bindings
