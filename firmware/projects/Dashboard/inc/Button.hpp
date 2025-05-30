#pragma once

#include <cstdint>

#include "shared/periph/gpio.hpp"

class Button {
public:
    Button(shared::periph::DigitalInput& input);

    bool IsPressed() const;
    bool PosEdge() const;
    bool NegEdge() const;
    uint32_t GetHeldDuration() const;

    void Update(int time_ms);

private:
    bool pos_edge_;
    bool neg_edge_;
    bool previous_state_;
    uint32_t last_change_time_;
    uint32_t last_update_time_;

    const uint32_t kDebounceDelay = 200;
    shared::periph::DigitalInput& input_;
};
