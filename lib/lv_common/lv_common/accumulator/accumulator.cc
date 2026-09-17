#include "accumulator.hpp"

#include "generated/can/veh_bus.hpp"
#include "generated/can/veh_messages.hpp"
#include "optional"
#include "periph/gpio.hpp"
namespace accumulator {

using namespace generated::can;
using macfe::periph::DigitalOutput;
enum Feedback : bool {
    // feedback is inverted relative to command
    OPEN = true,
    CLOSED = false,
};

static bool enabled = false;
static std::optional<RxContactor_Feedback> contactors;

void Controller::UpdateOutputs() {
    periph.accumulator_en->Set(enabled);
}

void Controller::Init(DigitalOutput& accumulator_en) {
    enabled = false;
    contactors = std::nullopt;

    periph = accumulator_periph{&accumulator_en};
    UpdateOutputs();
}

void Controller::SetEnabled(bool enable) {
    enabled = enable;
}

bool Controller::ConfirmContactorsOpen(void) {
    if (contactors.has_value()) {
        return contactors->Pack_Positive_Feedback() == OPEN &&
               contactors->Pack_Precharge_Feedback() == OPEN &&
               contactors->Pack_Negative_Feedback() == OPEN;
    } else {
        return false;
    }
}

bool Controller::IsRunning(void) {
    if (contactors.has_value()) {
        return contactors->Pack_Positive_Feedback() == CLOSED &&
               contactors->Pack_Precharge_Feedback() == OPEN &&
               contactors->Pack_Negative_Feedback() == CLOSED;
    } else {
        return false;
    }
}

void Controller::task_10hz(generated::can::VehBus& veh_can) {
    contactors = veh_can.GetRxContactor_Feedback();
    UpdateOutputs();
}

}  // namespace accumulator