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
extern shared::periph::CanBase& veh_can_base;

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

generated::can::VehMsgRegistry veh_can_registry{};

shared::can::CanBus veh_can_bus{
    bindings::veh_can_base,
    veh_can_registry,
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

    // CAN Messages
    generated::can::speedGoat_tx from_speedgoat;
    generated::can::fc_tx to_speedgoat;

    while (1) {
        // Read all input
        accel_pedal_pos_1_value = bindings::accel_pedal_pos_1.Read();
        accel_pedal_pos_2_value = bindings::accel_pedal_pos_2.Read();
        brake_pedal_pos_1_value = bindings::brake_pedal_pos_1.Read();
        brake_pedal_pos_2_value = bindings::brake_pedal_pos_2.Read();
        steering_angle_value = bindings::steering_angle.Read();
        hvil_feedback_value = bindings::hvil_feedback.Read();
        start_button_n_value = bindings::start_button_n.Read();

        // Pack CAN Message and send
        to_speedgoat.fc_start_button = !start_button_n_value;
        // TODO: this is random, it is a 12 bit adc so i chose down the middle
        // to be the cuttoff
        to_speedgoat.fc_hvil_status = hvil_feedback_value > 2048;
        to_speedgoat.fc_apps1 =
            adc_to_voltage.Evaluate(accel_pedal_pos_1_value);
        to_speedgoat.fc_apps2 =
            adc_to_voltage.Evaluate(accel_pedal_pos_2_value);
        to_speedgoat.fc_bpps1 =
            adc_to_voltage.Evaluate(brake_pedal_pos_1_value);
        to_speedgoat.fc_bpps2 =
            adc_to_voltage.Evaluate(brake_pedal_pos_2_value);
        to_speedgoat.fc_steering_angle =
            adc_to_voltage.Evaluate(steering_angle_value);
        veh_can_bus.Send(to_speedgoat);

        // Get values from Speedgoat CAN message
        veh_can_bus.ReadWithUpdate(from_speedgoat);

        bindings::brake_light_en.Set(from_speedgoat.sg_brake_light_en);
        bindings::rtds_en.Set(from_speedgoat.sg_driver_speaker);

        bindings::TickBlocking(10);
    }

    return 0;
}