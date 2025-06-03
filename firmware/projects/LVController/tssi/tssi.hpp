#pragma once

#include "generated/can/veh_bus.hpp"

/// @brief Tractive System Active Indicator
/// A light on the car that indicates if the car can drive or is faulted.
namespace tssi {

void SetEnabled(bool enable);

void task_10hz(generated::can::VehBus& veh_can);

}  // namespace tssi
