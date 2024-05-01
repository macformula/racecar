/// @author Samuel Parent
/// @date 2024-05-01

#include "mcal/raspi/periph/gpio.h"
#include "mcal/raspi/sil/sil_client.h"
#include "shared/periph/gpio.h"

const std::string ecu_name = "test_ecu";
const std::string server_addr = "localhost:31522";

namespace mcal {
using namespace raspi::periph;
using namespace raspi::sil;

SilClient sil_client(server_addr);

DigitalInput button_di{ecu_name, "button1"};
DigitalOutput indicator_do{ecu_name, "indicator1"};
}  // namespace mcal

namespace bindings {

const shared::periph::DigitalInput& button_di = mcal::button_di;
const shared::periph::DigitalOutput& indicator_do = mcal::indicator_do;

void Initialize() {
    std::cout << "Initializing SIL..." << std::endl;
    std::cout << "Server address: " << server_addr << std::endl;
}

}  // namespace bindings
