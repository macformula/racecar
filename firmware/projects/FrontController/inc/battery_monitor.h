#pragma once

// Add enum values if I missed any
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

enum class BmControlStatus {
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

struct BmInput {
    BmCmd cmd;
    ContactorState precharge_contactor_states;
    ContactorState hv_pos_contactor_states;
    ContactorState hv_neg_contactor_states;
    ContactorState hvil_status;
    float pack_soc;
};

struct ContactorCMD {
    ContactorState precharge_contactor_cmd;
    ContactorState hv_pos_contactor_cmd;
    ContactorState hv_neg_contactor_cmd;
};

struct BmOutput {
    BmStatus status;
    ContactorCMD contactor;
};

// Only DECLARE the class methods and variables here. Write the method
// definitions in the .cc file.
class BatteryMonitor {
public:
    // do not change the public interface
    BatteryMonitor();
    BmOutput update(const BmInput& input, int time_ms);

private:
    // add private methods / variables as necessary.
    BmStatus bmStatusUpdate(const BmInput& input, int time_ms);
    BmControlStatus bmControlTransition(BmStatus status, int time_ms);
    ContactorCMD setContactorCMD(BmControlStatus bm_control_status_);

    // State machine variables (BmUpdate)
    BmStatus current_status_;
    BmControlStatus bm_control_status_;
    int snapshot_time_ms_;
};
