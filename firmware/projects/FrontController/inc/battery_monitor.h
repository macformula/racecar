#pragma once
#include <optional>

enum class BmStatus {
    INIT,
    IDLE,
    STARTUP,
    INIT_PRECHARGE,
    PRECHARGE,
    RUNNING,
    HVIL_INTERRUPT,
    LOW_SOC,
};

enum class BmCmd {
    INIT,
    HV_STARTUP,
    HV_SHUTDOWN,
};

enum class ContactorState : bool {
    OPEN = false,
    CLOSED = true,
};

struct ContactorCMD {
    ContactorState precharge;
    ContactorState hv_positive;
    ContactorState hv_negative;
};

class BatteryMonitor {
public:
    struct Input {
        BmCmd cmd;
        ContactorState precharge_contactor_states;
        ContactorState hv_pos_contactor_states;
        ContactorState hv_neg_contactor_states;
        ContactorState hvil_status;
        float pack_soc;
    };
    struct Output {
        BmStatus status;
        ContactorCMD contactor;
    };

    BatteryMonitor();
    Output Update(const Input& input, int time_ms);

private:
    enum class ControlStatus {
        STARTUP_CMD,
        CLOSE_HV_NEG,
        CLOSE_PRECHARGE,
        CLOSE_HV_POS,
        OPEN_PRECHARGE,
    };
    std::optional<BmStatus> TransitionStatus(const Input& input, int time_ms);
    std::optional<ControlStatus> TransitionControl(BmStatus status,
                                                   int time_ms);
    ContactorCMD SelectContactorCmd(ControlStatus bm_control_status_);

    // State machine variables (BmUpdate)
    std::optional<BmStatus> current_status_;
    std::optional<ControlStatus> bm_control_status_;
    int status_snapshot_time_ms_;
    int control_snapshot_time_ms_;
};
