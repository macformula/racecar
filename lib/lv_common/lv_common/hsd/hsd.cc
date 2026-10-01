/// @author Manush Patel
/// @date 2026-05-23

#include "hsd.hpp"

#include "lib/lv_common/lv_common/alerts/alerts.hpp"

namespace macfe::lv {

static constexpr float kVoltsToMa_4ch = 1500.0f / 0.535f;    // HSD_1
static constexpr float kVoltsToMa_2ch = 10000.0f / 0.4815f;  // HSD_2–6
static constexpr float kFaultThresholdV = 0.64f;
static constexpr uint8_t kTotalChannels = 14;
Reading HSD2::Read(uint8_t channel) {
    _sel1.Set(channel & 0x01);
    _isense_en.SetHigh();
    float v = _isense.ReadVoltage();
    bool overthreshold =
        (v > kFaultThresholdV) &&
        (overthreshold && prev_tick_ch[channel] > kFaultThresholdV);
    prev_tick_ch[channel] = v;
    _isense_en.SetLow();

    return Reading{
        .current_ma = v * kVoltsToMa_2ch,
        .fault = overthreshold,
    };
}

Reading HSD4::Read(uint8_t channel) {
    _sel1.Set(channel & 0x01);
    _sel2.Set(channel & 0x02);
    _isense_en.SetHigh();
    float v = _isense.ReadVoltage();
    bool overthreshold =
        (v > kFaultThresholdV) &&
        (overthreshold && prev_tick_ch[channel] > kFaultThresholdV);
    prev_tick_ch[channel] = v;
    _isense_en.SetLow();

    return Reading{
        .current_ma = v * kVoltsToMa_4ch,
        .fault = overthreshold,
    };
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

bool HSD::HasOverCurrent() {
    for (int i = 0; i < channel_count; i++) {
        if (_channels[i].fault) {
            return true;
        }
    }
    return false;
}
}  // namespace macfe::lv