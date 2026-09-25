#pragma once

#include "generated/can/veh_bus.hpp"
#include "periph/gpio.hpp"
namespace macfe::lv {

class Accumulator {
public:
    Accumulator(macfe::periph::DigitalOutput& accumulator_en)
        : accumulator_en(accumulator_en) {
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
    macfe::periph::DigitalOutput& accumulator_en;
    bool enabled = false;
    std::optional<RxContactor_Feedback> contactors;
};

}  // namespace macfe::lv