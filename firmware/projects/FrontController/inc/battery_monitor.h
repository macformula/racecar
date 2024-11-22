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
};

struct BmInput {
    BmCmd cmd;
    bool precharge_contactor_states;
    bool hv_pos_contactor_states;
    bool hv_neg_contactor_states;
    bool hvil_status;
    float pack_soc;
};

struct BmOutput {
    BmStatus status;
    bool precharge_contactor_cmd;
    bool hv_pos_contactor_cmd;
    bool hv_neg_contactor_cmd;
};

// Only DECLARE the class methods an variables here. Write the method
// definitions in the .cc file.
class BatteryMonitor {
public:
    // do not change the public interface
    BatteryMonitor();
    BmOutput update(const BmInput& input, int time_ms);

private:
    // add private methods / variables as necessary.
};