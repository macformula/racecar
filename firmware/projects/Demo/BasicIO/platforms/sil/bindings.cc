/// @author Samuel Parent
/// @date 2024-05-01

#include "shared/periph/gpio.hpp"
#include "validation/sil/sil_client.h"

const std::string ecu_name = "DemoProject";
const std::string server_addr = "localhost:12345";

namespace val {
using namespace sil;
SilClient sil_client(server_addr);

}  // namespace val

namespace mcal {
using namespace raspi;

DigitalInput& button_di{ecu_name, "IndicatorButton", val::sil_client};
DigitalInput& indicator_do{ecu_name, "IndicatorLed", val::sil_client};
}  // namespace mcal

namespace bindings {

const shared::periph::DigitalInput& button_di = mcal::button_di;
const shared::periph::DigitalOutput& indicator_do = mcal::indicator_do;

void Initialize() {
    std::cout << "Initializing SIL..." << std::endl;
    std::cout << "Using address: " << server_addr << std::endl;

    val::sil_client.Connect();

    mcal::button_di.Register();
    mcal::indicator_do.Register();
}

}  // namespace bindings
