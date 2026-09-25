#include "accumulator.hpp"

#include "generated/can/veh_bus.hpp"
#include "generated/can/veh_messages.hpp"
#include "optional"
#include "periph/gpio.hpp"
namespace macfe::lv {

using namespace generated::can;
using macfe::periph::DigitalOutput;
enum Feedback : bool {
    // feedback is inverted relative to command
    OPEN = true,
    CLOSED = false,
};

void Accumulator::UpdateOutputs() {
    accumulator_en.Set(enabled);
}

void Accumulator::SetEnabled(bool enable) {
    enabled = enable;
}

bool Accumulator::ConfirmContactorsOpen(void) {
    if (contactors.has_value()) {
        return contactors->Pack_Positive_Feedback() == OPEN &&
               contactors->Pack_Precharge_Feedback() == OPEN &&
               contactors->Pack_Negative_Feedback() == OPEN;
    } else {
        return false;
    }
}

bool Accumulator::IsRunning(void) {
    if (contactors.has_value()) {
        return contactors->Pack_Positive_Feedback() == CLOSED &&
               contactors->Pack_Precharge_Feedback() == OPEN &&
               contactors->Pack_Negative_Feedback() == CLOSED;
    } else {
        return false;
    }
}

void Accumulator::task_10hz(generated::can::VehBus& veh_can) {
    contactors = veh_can.GetRxContactor_Feedback();
    UpdateOutputs();
}

}  // namespace macfe::lv