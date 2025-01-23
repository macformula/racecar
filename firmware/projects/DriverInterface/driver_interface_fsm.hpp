#pragma once

enum class DiCmd {
    INIT,
    HV_ON,
    READY_TO_DRIVE,
    SHUTDOWN,
    RUN_ERROR,
    ERR_RESET,
    UNKNOWN,
};

enum class DiSts {
    INIT,
    WAITING_FOR_DRVR,
    HV_START_REQ,
    MOTOR_START_REQ,
    RUNNING,
    ERR_COASTING,
    ERR,
};

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
