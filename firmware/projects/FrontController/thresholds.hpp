#pragma once

#include <cstdint>

namespace threshold {

// accumulator
const float PRECHARGE_COMPLETE_PERCENT = 90.f;
const float LOW_VOLTAGE_PERCENT = 30.f;
const float PACK_SHUTDOWN_PERCENT = 10.f;

}  // namespace threshold

namespace timeout {

using timeout_ms = uint32_t;

const timeout_ms PACK_VOLTAGE_UPDATE = 500;
const timeout_ms MIN_PRECHARGE_TIME = 6000;

}  // namespace timeout