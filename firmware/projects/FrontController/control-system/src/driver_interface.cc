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
    const double accel_lut_data[][2] = {
        {0., 0.},
        {1., 1},
    };
    static constexpr int lut_length =
        sizeof(accel_lut_data) / sizeof(accel_lut_data[0]);
    const shared::util::LookupTable<lut_length, double> accel_pedal_map{
        accel_lut_data};

    const double brake_lut_data[][2] = {
        {0., 0.},
        {1., 1},
    };
    static constexpr int brake_lut_length =
        sizeof(brake_lut_data) / sizeof(brake_lut_data[0]);
    const shared::util::LookupTable<brake_lut_length, double> brake_pedal_map{
        brake_lut_data};

    Output out;

    bool accel_pedal_1_error = !isInRange(input.accel_pedal_pos1);
    bool accel_pedal_2_error = !isInRange(input.accel_pedal_pos2);
    bool brake_pedal_error = !isInRange(input.brake_pedal_pos);
    bool steering_angle_error = !isInRange(input.steering_angle);

    double diff = std::abs(input.accel_pedal_pos1 - input.accel_pedal_pos2);
    bool accel_pedal_implausible = diff > 0.1;

    // bool di_error = accel_pedal_1_error || accel_pedal_2_error ||
    //                 brake_pedal_error || steering_angle_error ||
    //                 accel_pedal_implausible; // temp
    // bool di_error = accel_pedal_1_error;
    bool di_error = false;

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