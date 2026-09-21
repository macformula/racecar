#include "generated/can/veh_bus.hpp"
#include "periph/analog_input.hpp"
namespace suspension {
struct suspension_periph {
    macfe::periph::AnalogInput* suspension_travel3;
    macfe::periph::AnalogInput* suspension_travel4;
};
class Controller {
public:
    Controller(suspension_periph suspension_periph)
        : periph(suspension_periph) {};
    void task_10hz(generated::can::VehBus& veh_can);

private:
    suspension_periph periph;
    const float SENSOR_SUPPLY_V = 3.3f;
    float travel3 = 0.0f;
    float travel4 = 0.0f;

    void Measure(void) {}
};

}  // namespace suspension