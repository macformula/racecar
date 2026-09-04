#pragma once

#include <array>
#include <cstdint>
#include <span>

#include "etl/array.h"
#include "fan_controller.hpp"
#include "generated/can/veh_bus.hpp"
#include "periph/analog_input.hpp"
#include "temp_sensor.hpp"

namespace macfe::tms {

using ::generated::can::TxBmsBroadcast;
using ::generated::can::TxTMSValues;
using ::generated::can::VehBus;

inline constexpr size_t kSensorCount = 6;

TxTMSValues TMSBroadcast(const std::array<float, kSensorCount>& temperatures);

TxBmsBroadcast PackBmsBroadcast(
    const std::array<float, kSensorCount>& temperatures);

void ReadAndSendTemperatures(
    float update_period_ms, VehBus& veh_can_bus, FanController& fan_controller,
    etl::array<TempSensor, kSensorCount>& temp_sensors);
}  // namespace macfe::tms
