/// @author Samuel Parent
/// @date 2024-05-01

#include "mcal/raspi/periph/gpio.h"
#include "mcal/raspi/sil/sil_client.h"
#include "mcal/raspi/periph/can.h"
#include "mcal/raspi/periph/adc.h"
#include "shared/periph/gpio.h"

const std::string ecu_name = "FrontController";
const std::string server_addr = "localhost:31522";

namespace mcal {
using namespace raspi::periph;
using namespace raspi::sil;

SilClient sil_client(server_addr);

CanBase io_can_base{}


DigitalInput button_di{ecu_name, "IndicatorButton", sil_client};
DigitalOutput indicator_do{ecu_name, "IndicatorLed", sil_client};
}  // namespace mcal

namespace bindings {


void Initialize() {
    std::cout << "Initializing SIL..." << std::endl;
    std::cout << "Using address: " << server_addr << std::endl;

    mcal::sil_client.Connect();

    mcal::button_di.Register();
    mcal::indicator_do.Register();
}

}  // namespace bindings
