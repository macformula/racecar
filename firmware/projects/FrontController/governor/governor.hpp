#pragma once

#include <optional>

#include "../enums.hpp"

class Governor {
public:
    struct Input {
        AccSts acc_sts;
        MiSts mi_sts;
        DiSts di_sts;
    };
    struct Output {
        AccCmd acc_cmd;
        MiCmd mi_cmd;
        DiCmd di_cmd;
        GovSts gov_sts;
    };

    Governor();
    Output Update(const Input input, const int time_ms);

private:
    std::optional<GovSts> fsm_state_;  // nullopt = unitialized
    int state_entered_time_;

    Output output_;

    const int kMaxMotorStartAttempts = 5;
    int motor_start_count_;

    std::optional<GovSts> Transition(const Input input, const int time_ms);
};