#pragma once

#include <optional>

#include "enums.hpp"

class Governor {
public:
    struct Input {
        BmSts bm_sts;
        MiSts mi_sts;
        DiSts di_sts;
    };
    struct Output {
        BmCmd bm_cmd;
        MiCmd mi_cmd;
        DiCmd di_cmd;
        GovSts gov_sts;
    };

    Governor();
    Output Update(const Input input, const int time_ms);

private:
    Output state_;
    int state_entered_time_;
    bool is_first_update_;

    const int kMaxMotorStartAttempts = 5;
    int motor_start_count_;

    std::optional<GovSts> Transition(const Input input, const int time_ms);
};