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

inline constexpr size_t kSensorCount = 6;

// we need a tms-context object, we need to declare 10hz, and 1hz

struct TmsContext {
    std::span<macfe::periph::AnalogInput* const, kSensorCount> adc_channels;
    etl::array<TempSensor, kSensorCount> temp_sensors;
    FanController& fan_controller;
    ::generated::can::VehBus& veh_can_bus;
};

// Read 6 thermistors, average them, compute BMS Broadcast, send can and adjusts
// fan
void Process10HzStep(TmsContext& ctx, float dt_ms);

void Process1HzStep(::generated::can::VehBus& veh_can_bus);
}  // namespace macfe::tms