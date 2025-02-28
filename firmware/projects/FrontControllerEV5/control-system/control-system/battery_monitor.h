#pragma once
#include <optional>

#include "enums.hpp"

enum class ControlStatus {
    STARTUP_CMD,
    CLOSE_HV_NEG,
    CLOSE_PRECHARGE,
    CLOSE_HV_POS,
    OPEN_PRECHARGE,
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
        float pack_soc;
    };
    struct Output {
        BmSts status;
        ControlStatus control_status;
        ContactorCMD contactor;
    };

    BatteryMonitor();
    Output Update(const Input& input, int time_ms);

private:
    std::optional<BmSts> TransitionStatus(const Input& input, int time_ms);
    std::optional<ControlStatus> TransitionControl(BmSts status, int time_ms);
    ContactorCMD SelectContactorCmd(ControlStatus bm_control_status_);

    // State machine variables (BmUpdate)
    std::optional<BmSts> current_status_;
    std::optional<ControlStatus> bm_control_status_;
    int status_snapshot_time_ms_;
    int control_snapshot_time_ms_;
};
