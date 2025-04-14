#include "control-system/driver_interface.hpp"

#include <cmath>

#include "control-system/driver_interface_fsm.hpp"
#include "shared/util/mappers/lookup_table.hpp"

template <typename T>
bool isInRange(T fractionIn) {
    return (fractionIn >= 0 && fractionIn <= 1);
}

DriverInterface::Output DriverInterface::Update(const Input input,
                                                const int time_ms) {
    using shared::util::LookupTable;

    auto apps_pedal_map = std::to_array<LookupTable<float>::Entry>({
        {0, 0},
        {1, 1},
    });
    LookupTable accel_pedal_map{apps_pedal_map};

    auto bpps_pedal_map = std::to_array<LookupTable<float>::Entry>({
        {0, 0},
        {1, 1},
    });
    LookupTable brake_pedal_map{bpps_pedal_map};

    Output out;

    bool accel_pedal_1_error = !isInRange(input.accel_pedal_pos1);
    bool accel_pedal_2_error = !isInRange(input.accel_pedal_pos2);
    bool brake_pedal_error = !isInRange(input.brake_pedal_pos);
    bool steering_angle_error = !isInRange(input.steering_angle);

    double diff = std::abs(input.accel_pedal_pos1 - input.accel_pedal_pos2);
    bool accel_pedal_implausible = diff > 0.1;

    bool di_error = accel_pedal_1_error || accel_pedal_2_error ||
                    brake_pedal_error || steering_angle_error ||
                    accel_pedal_implausible;

    out.brake_light_en = input.brake_pedal_pos > 0.1 && !brake_pedal_error;

    if (brake_pedal_error) {
        out.brake_pedal_position = 0;
    } else {
        out.brake_pedal_position =
            brake_pedal_map.Evaluate(input.brake_pedal_pos);
    }

    out.steering_angle = steering_angle_error ? 0.5 : input.steering_angle;

    DiFsm::Output fsm_output = di_fsm.Update(
        {
            .command = input.di_cmd,
            .driver_button = input.driver_button,
            .brake_pedal_pos = input.brake_pedal_pos,
            .di_error = di_error,
        },
        time_ms);
    out.di_sts = fsm_output.status;
    out.driver_speaker = fsm_output.speaker_enable;

    if (fsm_output.ready_to_drive && !di_error) {
        out.driver_torque_req =
            accel_pedal_map.Evaluate(input.accel_pedal_pos1);
    } else {
        out.driver_torque_req = 0;
    }

    return out;
}