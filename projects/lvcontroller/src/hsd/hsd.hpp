/// @author Manush Patel
/// @date 2026-05-23

#pragma once
#include <cstdint>

#include "generated/can/veh_bus.hpp"
#include "periph/analog_input.hpp"
#include "periph/gpio.hpp"

namespace hsd {

using macfe::periph::AnalogInput;
using macfe::periph::DigitalOutput;

struct Reading {
    float current_ma = 0.0f;
    bool fault = false;
};

class HSD {
public:
    virtual Reading Read(uint8_t channel) = 0;
    virtual ~HSD() = default;
};

void Update_10Hz(generated::can::VehBus& veh_can);

bool HasOverCurrent();
float GetCurrent(uint8_t channel_index);
bool IsOverCurrent(uint8_t channel_index);

class HSD2Channel : public HSD {
public:
    HSD2Channel(AnalogInput& isense, DigitalOutput& en, DigitalOutput& sel)
        : isense_(isense), en_(en), sel_(sel) {}

    Reading Read(uint8_t channel) override;

private:
    AnalogInput& isense_;
    DigitalOutput& en_;
    DigitalOutput& sel_;
};

class HSD4Channel : public HSD {
public:
    HSD4Channel(AnalogInput& isense, DigitalOutput& en, DigitalOutput& sel0,
                DigitalOutput& sel1)
        : isense_(isense), en_(en), sel0_(sel0), sel1_(sel1) {}

    Reading Read(uint8_t channel) override;

private:
    AnalogInput& isense_;
    DigitalOutput& en_;
    DigitalOutput& sel0_;
    DigitalOutput& sel1_;
};

}  // namespace hsd
