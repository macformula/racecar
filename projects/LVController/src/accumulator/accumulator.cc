#include "accumulator.hpp"

#include "generated/can/veh_bus.hpp"
#include "generated/can/veh_messages.hpp"
#include "inc/bindings.hpp"

namespace accumulator {

using namespace generated::can;

enum Feedback : bool {
    // feedback is inverted relative to command
    OPEN = true,
    CLOSED = false,
};

static bool enabled = false;
static std::optional<RxContactor_Feedback> contactors;

static void UpdateOutputs(void) {
    bindings::accumulator_en.Set(enabled);
}

void Init(void) {
    enabled = false;
    contactors = std::nullopt;

    UpdateOutputs();
}

void SetEnabled(bool enable) {
    enabled = enable;
}

bool ConfirmContactorsOpen(void) {
    if (contactors.has_value()) {
        return contactors->Pack_Positive_Feedback() == OPEN &&
               contactors->Pack_Precharge_Feedback() == OPEN &&
               contactors->Pack_Negative_Feedback() == OPEN;
    } else {
        return false;
    }
}

bool IsRunning(void) {
    if (contactors.has_value()) {
        return contactors->Pack_Positive_Feedback() == CLOSED &&
               contactors->Pack_Precharge_Feedback() == OPEN &&
               contactors->Pack_Negative_Feedback() == CLOSED;
    } else {
        return false;
    }
}

void task_10hz(generated::can::VehBus& veh_can) {
    contactors = veh_can.GetRxContactor_Feedback();
    UpdateOutputs();
}

}  // namespace accumulator