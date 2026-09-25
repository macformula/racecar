#include "generated/can/veh_bus.hpp"
#include "periph/analog_input.hpp"
namespace suspension {

void task_10hz(generated::can::VehBus& veh_can,
               macfe::periph::AnalogInput& suspension_travel3,
               macfe::periph::AnalogInput& suspension_travel4);

}  // namespace suspension