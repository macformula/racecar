#include "driver_interface.hpp"
#include "shared/controls/driver_interface_error_handling.h"
#include "shared/controls/driver_interface_fsm.hpp"
#include "shared/controls/steering_angle.h"

// include "shared/controls/..." header files here

DriverInterface::DriverInterface() {}

// implement the block in here to compute all of the `output` fields
DiOutput DriverInterface::Update(DiInput input, int time_ms) {
    // Compute all potentiometer blocks here and define all outputs from it
    float accel_pedal_1_pos;
    bool accel_pedal_1_error;
    float accel_pedal_2_pos;
    bool accel_pedal_2_error;
    float front_brake_pedal_pres;
    bool front_brake_pedal_error;
    float rear_brake_pedal_pos;
    bool rear_brake_pedal_error;
    float steering_angle;
    bool steering_error;

    // Compute accelerator plausability check block
    bool accel_pedal_implausible;

    // Compute driver interface error block
    // QUESTION: Should there be an extra parameter in this function definition
    // to take the implausible input?
    bool di_error =
        ctrl::b_DriverInterfaceError(accel_pedal_1_error, accel_pedal_2_error,
                                     front_brake_pedal_error,
                                     rear_brake_pedal_error, steering_error) ||
        accel_pedal_implausible;

    // Compute max computation to pass into other blocks
    float brake_pedal_pos =
        std::max(front_brake_pedal_pres, rear_brake_pedal_pos);

    // Compute FSM block
    DiFsm::Input di_fsm_input{.command = input.di_cmd,
                              .driver_button = input.driver_button,
                              .brake_pedal_pos = brake_pedal_pos,
                              .di_error = di_error};
    DiFsm::Output di_fsm_output = di_fsm.Update(di_fsm_input, time_ms);

    // Compute brake pedal pos and brake light en
    bool brake_light_en;
    bool brakeError = !ctrl::isInRange(front_brake_pedal_pres) ||
                      !ctrl::isInRange(rear_brake_pedal_pos);

    if (brakeError) {
        brake_pedal_pos = 0;
    }

    //     driver_brake_pedal_position =
    //         pedal_map.Evaluate(input.brake_pedal_position);

    //     brake_light_en = (output.driver_brake_pedal_position > 0);
    // }

    // Compute steering angle
    bool steering_angle_error = !ctrl::isInRange(input.steering_angle);

    if (steering_angle_error) {
        steering_angle = 0.5;
    } else {
        steering_angle = steering_angle;
    }

    // compute driver torque request
    float driver_torque_request;
    //  if (ready_to_drive == 1 || driver_interface_error == 1) {
    //     if (u1 == 1) {
    //         output.driver_torque_req = 0;
    //     } else {
    //         output.driver_torque_req =
    //             pedal_map.Evaluate(input.accel_pedal_pos1)
    //     }
    // }

    // const double lut_data[][2] = {
    //     {0., 0.}, {100., 100.}};  // LUT table that maps from 0 to 100

    // constexpr int lut_length = sizeof(lut_data) / sizeof(lut_data[0]);

    // shared::util::LookupTable<lut_length, double> pedal_map{lut_data};

    return DiOutput{
        .di_sts = di_fsm_output.status,
        .driver_torque_req = driver_torque_request,
        .steering_angle = steering_angle,
        .brake_pedal_position = brake_pedal_pos,
        .driver_speaker = di_fsm_output.speaker_enable,
        .brake_light_en = brake_light_en,
    };
}