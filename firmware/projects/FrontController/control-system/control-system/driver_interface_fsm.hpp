#pragma once

#include "../../generated/can/veh_messages.hpp"
#include "enums.hpp"

class DiFsm {
public:
    struct Input {
        DiCmd command;
        generated::can::dashState driver_state;
        float brake_pedal_pos;
        bool di_error;
    };
    struct Output {
        DiSts status;
        bool speaker_enable;
        bool ready_to_drive;
    };

    DiFsm(DiSts initial_state = DiSts::INIT);
    Output Update(const Input input, const int time_ms);

private:
    DiSts Transition(const Input input, const int time_ms);

    DiSts status_;
    int speaker_start_time_;
};
