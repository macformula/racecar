/// @author Blake Freer
/// @date 2023-12-25

#include "bindings.h"

#include <iostream>

namespace bindings {

mcal::periph::DigitalInput button_di{1};
mcal::periph::DigitalOutput indicator_do{2};

void Initialize() {
    std::cout << "Initializing Windows..." << std::endl;
}

}  // namespace bindings
