#pragma once

#include "enums.hpp"

class DiFsm {
public:
    struct Output {
        DiSts status;
        bool speaker_enable;
        bool ready_to_drive;
    };
    struct Input {
        DiCmd command;
        bool driver_button;
        float brake_pedal_pos;
        bool di_error;
    };

    DiFsm(DiSts initial_state = DiSts::INIT);
    Output Update(const Input input, const int time_ms);

private:
    DiSts Transition(const Input input, const int time_ms);

    DiSts status_;
    int speaker_start_time_;
};
