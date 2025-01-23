#include <iostream>

#include "driver_interface.hpp"
#include "shared/controls/brake_pedal_lights.h"
#include "shared/controls/driver_interface_error_handling.h"
#include "shared/controls/driver_interface_fsm.hpp"
#include "shared/controls/steering_angle.h"
#include "shared/util/mappers/lookup_table.hpp"

// include "shared/controls/..." header files here

DriverInterface::DriverInterface() {}

// implement the block in here to compute all of the `output` fields
DiOutput DriverInterface::Update(DiInput input, int time_ms) {
    float accel_pedal_1_pos = input.accel_pedal_pos1;
    bool accel_pedal_1_error;
    float accel_pedal_2_pos = input.accel_pedal_pos2;
    bool accel_pedal_2_error;
    float front_brake_pedal_pres = input.front_brake_pedal_position;
    bool front_brake_pedal_error;
    float rear_brake_pedal_pos = input.rear_brake_pedal_position;
    bool rear_brake_pedal_error;
    float steering_angle = input.steering_angle;
    bool steering_error;

    // Compute all potentiometer blocks here and define all outputs from it

    if (accel_pedal_1_pos >= 0 && accel_pedal_1_pos <= 1.00) {
        accel_pedal_1_error = false;

    } else {
        accel_pedal_1_error = true;
    }

    if (accel_pedal_2_pos >= 0 && accel_pedal_2_pos <= 1.00) {
        accel_pedal_1_error = false;
    } else {
        accel_pedal_2_error = true;
    }

    if (front_brake_pedal_pres >= 0 && front_brake_pedal_pres <= 1.00) {
        front_brake_pedal_error = false;
    } else {
        front_brake_pedal_error = true;
    }

    if (rear_brake_pedal_pos >= 0 && rear_brake_pedal_pos <= 1.00) {
        rear_brake_pedal_error = false;
    } else {
        rear_brake_pedal_error = true;
    }

    bool steering_angle_error = !ctrl::isInRange(steering_angle);

    // Compute accelerator plausability check block
    bool accel_pedal_implausible;

    double diff = std::abs(accel_pedal_1_pos - accel_pedal_2_pos);

    if (diff > 0.1 * accel_pedal_1_pos) {
        accel_pedal_implausible = true;
    } else {
        accel_pedal_implausible = false;
    }

    bool di_error =
        ctrl::b_DriverInterfaceError(accel_pedal_1_error, accel_pedal_2_error,
                                     front_brake_pedal_error,
                                     rear_brake_pedal_error, steering_error) ||
        accel_pedal_implausible;

    // Compute max computation to pass into other blocks
    float brake_pedal_pos =
        std::max(front_brake_pedal_pres, rear_brake_pedal_pos);

    // Compute FSM block
    bool brake_light_en;
    DiFsm::Input di_fsm_input{.command = input.di_cmd,
                              .driver_button = input.driver_button,
                              .brake_pedal_pos = brake_pedal_pos,
                              .di_error = di_error};
    DiFsm::Output di_fsm_output = di_fsm.Update(di_fsm_input, time_ms);

    // Compute brake pedal pos and brake light en

    bool brakeError = !ctrl::isInRange(front_brake_pedal_pres) ||
                      !ctrl::isInRange(rear_brake_pedal_pos);

    if (brakeError) {
        brake_light_en = ctrl::DetectBrakeLight(brake_pedal_pos);
        brake_pedal_pos = 0;
    }

    // Compute steering angle
    steering_angle_error = !ctrl::isInRange(steering_angle);

    if (steering_angle_error) {
        steering_angle = 0.5;
    } else {
        steering_angle = steering_angle;
    }

    const double lut_data[][2] = {
        {0., 0.}, {100., 100.}};  // LUT table that maps from 0 to 100

    constexpr int lut_length = sizeof(lut_data) / sizeof(lut_data[0]);

    shared::util::LookupTable<lut_length, double> pedal_map{lut_data};

    // compute driver torque request
    float driver_torque_request;
    if (!di_fsm_output.ready_to_drive || di_error) {
        driver_torque_request = 0;
    } else {
        driver_torque_request = pedal_map.Evaluate(accel_pedal_1_pos);
    }

    return DiOutput{
        .di_sts = di_fsm_output.status,
        .driver_torque_req = driver_torque_request,
        .steering_angle = steering_angle,
        .brake_pedal_position = brake_pedal_pos,
        .driver_speaker = di_fsm_output.speaker_enable,
        .brake_light_en = brake_light_en,
    };
}