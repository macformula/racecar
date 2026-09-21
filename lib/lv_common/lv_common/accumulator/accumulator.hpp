#pragma once

#include "generated/can/veh_bus.hpp"
#include "periph/gpio.hpp"
namespace accumulator {

struct accumulator_periph {
    macfe::periph::DigitalOutput* accumulator_en;
};

class Controller {
public:
    Controller(accumulator_periph accumulator_periph)
        : periph(accumulator_periph) {
        enabled = false;
        contactors = std::nullopt;

        UpdateOutputs();
    };

    void SetEnabled(bool enable);

    bool ConfirmContactorsOpen(void);
    bool IsRunning(void);

    void task_10hz(generated::can::VehBus& veh_can);

private:
    void UpdateOutputs();
    accumulator_periph periph;
    bool enabled = false;
    std::optional<RxContactor_Feedback> contactors;
};

}  // namespace accumulator