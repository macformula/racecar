/// @author Manush Patel
/// @date 2026-05-23

#include "hsd.hpp"

#include "fc_common/alerts/alerts.hpp"

namespace macfe::fc {

static constexpr float kFaultThresholdV_4ch = 0.64f;
static constexpr float kFaultThresholdV_1ch = 0.48f;
static constexpr float kVoltsToMa_4ch = 1500.0f / 0.535f;   // HSD_1
static constexpr float kVoltsToMa_1ch = 4000.0f / 0.3424f;  // HSD_2

static constexpr uint8_t kTotalChannels = 5;
static Reading channels[kTotalChannels];

Reading HSD1::Read(uint8_t channel) {
    _isense_en.SetHigh();
    float v = _isense.ReadVoltage();
    _isense_en.SetLow();

    return Reading{.current_ma = v * kVoltsToMa_1ch,
                   .fault = (v > kFaultThresholdV_1ch)};
}

Reading HSD4::Read(uint8_t channel) {
    _sel1.Set(channel & 0x01);
    _sel2.Set(channel & 0x02);
    _isense_en.SetHigh();
    float v = _isense.ReadVoltage();
    bool overthreshold = (v > kFaultThresholdV_4ch &&
                          prev_tick_ch[channel] > kFaultThresholdV_4ch);
    prev_tick_ch[channel] = v;
    _isense_en.SetLow();

    return Reading{
        .current_ma = v * kVoltsToMa_4ch,
        .fault = overthreshold,
    };
}

bool HSD::HasOverCurrent() {
    for (const Reading& r : channels) {
        if (r.fault) {
            return true;
        }
    }
    return false;
}

void HSD::Update_10Hz(generated::can::VehBus&) {
    uint8_t channel_index = 0;

    for (uint8_t hsd_index = 0; hsd_index < hsd_count; ++hsd_index) {
        for (uint8_t channel = 0; channel < channels_per_hsd[hsd_index];
             ++channel) {
            _channels[channel_index++] = _hsds[hsd_index]->Read(channel);
        }
    }

    alerts::Get().hsd_overcurrent = HasOverCurrent();
}

bool IsOverCurrent(uint8_t channel_index) {
    if (channel_index >= kTotalChannels) return false;
    return channels[channel_index].fault;
}

float GetCurrent(uint8_t channel_index) {
    if (channel_index >= kTotalChannels) return 0.0f;

    return channels[channel_index].current_ma;
}

}  // namespace macfe::fc
