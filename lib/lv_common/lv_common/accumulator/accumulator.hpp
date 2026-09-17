#pragma once

#include "generated/can/veh_bus.hpp"
#include "periph/gpio.hpp"
namespace accumulator {

struct accumulator_periph {
    macfe::periph::DigitalOutput* accumulator_en;
};

class Controller {
public:
    void Init(macfe::periph::DigitalOutput& accumulator_en);

    void SetEnabled(bool enable);

    bool ConfirmContactorsOpen(void);
    bool IsRunning(void);

    void task_10hz(generated::can::VehBus& veh_can);

private:
    void UpdateOutputs();
    accumulator_periph periph;
};

}  // namespace accumulator