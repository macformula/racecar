/// @author Manush Patel
/// @date 2026-05-23

#include "hsd.hpp"

#include "lib/lv_common/lv_common/alerts/alerts.hpp"

namespace hsd {

// Read indexed from 1
// Channel indexed from 0
Reading Controller::Read(uint8_t read_hsd, uint8_t channel) {
    hsd hsd = periph.hsd[read_hsd];
    for (int i = 0; i < hsd.num_sel; i++) {
        hsd.sel[i]->Set(channel & (2 << i));
    }
    hsd.isense_en->SetHigh();
    float v = hsd.isense->ReadVoltage();
    bool overthreshold = (v > kFaultThresholdV);

    if (overthreshold && hsd.prev_tick_ch[channel] > kFaultThresholdV) {
        // greater than threshold voltage > 100ms, detect fault
        overthreshold = true;
    } else {
        overthreshold = false;
    }
    hsd.prev_tick_ch[channel] = v;
    hsd.isense_en->SetLow();

    return Reading{
        .current_ma = v * kVoltsToMa_2ch,
        .fault = overthreshold,
    };
}

void Controller::Update_10Hz(generated::can::VehBus&) {
    // HSD1 - 4 channels
    channels[0] = Read(1, 0);
    channels[1] = Read(1, 1);
    channels[2] = Read(1, 2);
    channels[3] = Read(1, 3);

    // HSD2 through HSD5 — 2 channels each
    channels[4] = Read(2, 0);
    channels[5] = Read(2, 1);

    channels[6] = Read(3, 0);
    channels[7] = Read(3, 1);

    channels[8] = Read(4, 0);
    channels[9] = Read(4, 1);

    channels[10] = Read(5, 0);
    channels[11] = Read(5, 1);

    // HSD6 — 2 channel
    channels[12] = Read(6, 0);
    channels[13] = Read(6, 1);

    bool any_fault = HasOverCurrent();
    alerts::Get().hsd_overcurrent = any_fault;
}

bool Controller::HasOverCurrent() {
    for (int i = 0; i < channel_num; i++) {
        if (channels[i].fault) {
            return true;
        }
    }
    return false;
}
}  // namespace hsd