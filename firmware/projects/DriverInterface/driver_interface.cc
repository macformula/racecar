#include <chrono>
#include <thread>

#include "driver_interface.hpp"
#include "shared/controls/driver_interface_fsm.hpp"
#include "shared/controls/steering_angle.hpp"
#include "shared/error_handling/driver_interface_error_handling.hpp"
// include "shared/controls/..." header files here

DiOutput DriverInterface::Update(DiInput input, int time_ms) {
    DiOutput output;

    // implement the block in here to compute all of the `output` fields
    output.brake_pedal_position =
        std::max(input.front_brake_pressure, input.rear_brake_pressure);
    // initialization state

    DiInput::Input in {
        ready_to_drive = false;
        driver_speaker = false;
        GOV_E_DISTS = DI_STATUSES.DI_STS_INIT;
    }
    bool driverInterfaceError = b_DriverInterfaceError(
        input.accel_pedal_pos1, input.accel_pedal_pos2,
        input.front_brake_pressure, input.rear_brake_pressure,
        input.raw_steering_angle);

    while (driverInterfaceError == 0) {
        driverInterfaceError = b_DriverInterfaceError(
            input.accel_pedal_pos1, input.accel_pedal_pos2,
            input.front_brake_pressure, input.rear_brake_pressure,
            input.raw_steering_angle);  // determines if components are in
                                        // range, returns error if so
        switch (GOV_E_DICMD) {
            case DI_CMD_INIT:
                GOV_E_DISTS = DI.STATUSES.WAITING_FOR_DRVR;
                if (DI_b_DriverButton == 1) {
                    GOV_E_DISTS = DI.STATUSES.HV_START_REQ;
                }
                break;

            case (DI_CMD.HV_ON):
                if (DI_b_DriverButton == 1) {
                    GOV_E_DISTS = DI.STATUSES.READY_TO_DRIVE;
                }
                break;
            case (DI_CMD.READY_TO_DRIVE):
                if (input.brake_pedal_position > 0.1) {
                    ready_to_drive = true;
                    GOV_E_DISTS = DI_STATUSES.RUNNING;
                    driver_speaker = true;
                    std::this_thread::sleep_for(
                        std::chrono::seconds(2));  // wait two seconds
                    driver_speaker = false;
                }
                break;
            case (DI.CMD.RUN_ERROR):
                ready_to_drive = false;
                GOV_E_DISTS = DI_STATUSES.DI_IDLE;
                break;

            case (DI_STATUSES.SHUTDOWN):
                GOV_E_DISTS = DI.STATUSES.DI_STS_INIT;
                ready_to_drive = false;
                driver_speaker = false;
                break;

            default:
                GOV_E_DISTS = DI.STATUSES.UNKNOWN;
                break;
        }
        if (!driverInterfaceError) break;
    }

    if (driverInterfaceError) {
        GOV_E_DISTS = DI_STATUSES.di_error;
    }

    const double lut_data[][2] = {
        {0., 0.}, {100., 100.}};  // LUT table that maps from 0 to 100

    constexpr int lut_length = sizeof(lut_data) / sizeof(lut_data[0]);

    shared::util::LookupTable<lut_length, double> pedal_map{lut_data};

    if (ready_to_drive == 1 || driverInterfaceError == 1) {
        if (u1 == 1) {
            output.driver_torque_req = 0;
        } else {
            output.driver_torque_req =
                pedal_map.Evaluate(input.accel_pedal_pos1)
        }
    }
    bool brakeError = !isInRange(input.front_brake_pressure) ||
                      !isInRange(input.rear_brake_pressure);

    if (brakeError) {
        output.brake_pedal_position = 0;
    } else {
        output.brake_pedal_position =
            std::max(input.front_brake_pressure, input.rear_brake_pressure);

        output.driver_brake_pedal_position =
            pedal_map.Evaluate(input.brake_pedal_position);

        output.brake_light_en = (output.driver_brake_pedal_position > 0);
    }
    bool steering_angle_error = !isInRange(input.steering_angle);

    if (steering_angle_error) {
        output.steering_angle = 0.5;
    } else {
        output.steering_angle = input.steering_angle;
    }

    output.driver_speaker = driver_speaker;
    return output;
}