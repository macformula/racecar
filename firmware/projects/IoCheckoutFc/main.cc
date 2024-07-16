/// @author Samuel Parent
/// @date 2024-05-05

#include <cstdint>

#include "generated/can/can_messages.h"
#include "generated/can/msg_registry.h"
#include "shared/comms/can/can_bus.h"
#include "shared/periph/adc.h"
#include "shared/periph/gpio.h"
#include "shared/util/mappers/linear_map.h"

namespace bindings {
extern shared::periph::CanBase& io_can_base;

extern shared::periph::ADCInput& accel_pedal_pos_1;
extern shared::periph::ADCInput& accel_pedal_pos_2;
extern shared::periph::ADCInput& brake_pedal_pos_1;
extern shared::periph::ADCInput& brake_pedal_pos_2;
extern shared::periph::ADCInput& suspension_travel_1;
extern shared::periph::ADCInput& suspension_travel_2;
extern shared::periph::ADCInput& hvil_feedback;
extern shared::periph::ADCInput& steering_angle;

extern shared::periph::DigitalInput& start_button_n;
extern shared::periph::DigitalInput& wheel_speed_left_a;
extern shared::periph::DigitalInput& wheel_speed_left_b;
extern shared::periph::DigitalInput& wheel_speed_right_a;
extern shared::periph::DigitalInput& wheel_speed_right_b;

extern shared::periph::DigitalOutput& debug_led_en;
extern shared::periph::DigitalOutput& dashboard_en;
extern shared::periph::DigitalOutput& hvil_led_en;
extern shared::periph::DigitalOutput& brake_light_en;
extern shared::periph::DigitalOutput& status_led_en;
extern shared::periph::DigitalOutput& rtds_en;

extern void Initialize();
extern void TickBlocking(uint32_t);
}  // namespace bindings

/***************************************************************
    Objects
***************************************************************/

generated::can::IoMsgRegistry io_can_registry{};

shared::can::CanBus io_can_bus{
    bindings::io_can_base,
    io_can_registry,
};

shared::util::LinearMap<float, uint32_t> adc_to_voltage{
    static_cast<float>(3.3 / 4095.0), 0};

int main(void) {
    bindings::Initialize();

    // Digital input values
    bool start_button_n_value;
    bool wheel_speed_left_a_value;
    bool wheel_speed_left_b_value;
    bool wheel_speed_right_a_value;
    bool wheel_speed_right_b_value;

    // Adc values
    uint32_t accel_pedal_pos_1_value;
    uint32_t accel_pedal_pos_2_value;
    uint32_t brake_pedal_pos_1_value;
    uint32_t brake_pedal_pos_2_value;
    uint32_t suspension_travel_1_value;
    uint32_t suspension_travel_2_value;
    uint32_t hvil_feedback_value;
    uint32_t steering_angle_value;

    // Tx Messages
    generated::can::PedalSensors pedal_sensors_msg;
    generated::can::Wheels wheels_msg;
    generated::can::VehicleInfo vehilcle_info_msg;
    generated::can::Suspension suspension_msg;

    bindings::brake_light_en.SetHigh();
    while (1) {
        // Update IO CAN bus
        io_can_bus.Update();

        // Read all analog input
        accel_pedal_pos_1_value = bindings::accel_pedal_pos_1.Read();
        accel_pedal_pos_2_value = bindings::accel_pedal_pos_2.Read();
        brake_pedal_pos_1_value = bindings::brake_pedal_pos_1.Read();
        brake_pedal_pos_2_value = bindings::brake_pedal_pos_2.Read();
        suspension_travel_1_value = bindings::suspension_travel_1.Read();
        suspension_travel_2_value = bindings::suspension_travel_2.Read();
        hvil_feedback_value = bindings::hvil_feedback.Read();
        steering_angle_value = bindings::steering_angle.Read();

        // Read all digital input
        start_button_n_value = bindings::start_button_n.Read();
        wheel_speed_left_a_value = bindings::wheel_speed_left_a.Read();
        wheel_speed_left_b_value = bindings::wheel_speed_left_b.Read();
        wheel_speed_right_a_value = bindings::wheel_speed_right_a.Read();
        wheel_speed_right_b_value = bindings::wheel_speed_right_b.Read();

        // Pack Can messasges
        pedal_sensors_msg.apps1 = accel_pedal_pos_1_value;
        pedal_sensors_msg.apps2 = accel_pedal_pos_2_value;
        pedal_sensors_msg.bpps1 = brake_pedal_pos_1_value;
        pedal_sensors_msg.bpps2 = brake_pedal_pos_2_value;

        wheels_msg.wheel_speed_left_a = wheel_speed_left_a_value;
        wheels_msg.wheel_speed_left_b = wheel_speed_left_b_value;
        wheels_msg.wheel_speed_right_a = wheel_speed_right_a_value;
        wheels_msg.wheel_speed_right_b = wheel_speed_right_b_value;
        wheels_msg.steering_angle =
            adc_to_voltage.Evaluate(steering_angle_value);

        vehilcle_info_msg.start_button = !start_button_n_value;
        vehilcle_info_msg.hvil_feedback =
            adc_to_voltage.Evaluate(hvil_feedback_value);

        suspension_msg.suspension_travel1 =
            adc_to_voltage.Evaluate(suspension_travel_1_value);
        suspension_msg.suspension_travel2 =
            adc_to_voltage.Evaluate(suspension_travel_2_value);

        // Transmit messages
        io_can_bus.Send(pedal_sensors_msg);
        io_can_bus.Send(wheels_msg);
        io_can_bus.Send(vehilcle_info_msg);
        io_can_bus.Send(suspension_msg);

        // Wait 100ms
        bindings::TickBlocking(100);
    }

    return 0;
}