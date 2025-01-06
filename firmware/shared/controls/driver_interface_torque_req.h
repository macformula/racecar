#pragma once
namespace ctrl {

// Templating means this function will work on any types that support addition.
template <typename T>
T driver_interface_torque_req(bool ready_to_drive, bool driver_int_error,
                              T accel_ped_pos) {
    if ((!ready_to_drive) || driver_int_error) {
        return 0;
    } else {
        return accel_ped_pos;
    }
}

}  // namespace ctrl