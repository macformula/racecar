#pragma once

#include "generated/can/veh_bus.hpp"
#include "periph/gpio.hpp"

namespace brake_light {
struct brake_light_periph {
    macfe::periph::DigitalOutput* brake_light_en;
};
class Controller {
public:
    void Init(macfe::periph::DigitalOutput& brake_light_en);

    void task_100hz(generated::can::VehBus& veh_can);

private:
    brake_light_periph periph;
};
}  // namespace brake_light