#pragma once

#include <optional>

#include "enums.hpp"

enum class ContactorState : bool {
    OPEN = false,
    CLOSED = true,
};

struct ContactorCMD {
    ContactorState precharge;
    ContactorState positive;
    ContactorState negative;
};

class BatteryMonitor {
public:
    struct Input {
        BmCmd cmd;
        ContactorState precharge_contactor_states;
        ContactorState pos_contactor_states;
        ContactorState neg_contactor_states;
        float pack_soc;
    };
    struct Output {
        BmSts status;
        ContactorCMD contactor;
    };

    BatteryMonitor();
    Output Update(const Input& input, int time_ms);

private:
    std::optional<BmSts> TransitionStatus(const Input& input, int time_ms);
    ContactorCMD SelectContactorCmd(BmSts status);

    // State machine variables (BmUpdate)
    std::optional<BmSts> current_status_;
    int status_snapshot_time_ms_;
};
