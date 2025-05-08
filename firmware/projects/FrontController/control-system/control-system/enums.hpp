#pragma once

#include "../../generated/can/veh_messages.hpp"

enum class BmCmd {
    INIT,
    STARTUP,
};

enum class BmSts {
    INIT,
    STARTUP_ENSURE_OPEN,
    STARTUP_CLOSE_NEG,
    STARTUP_HOLD_CLOSE_NEG,
    STARTUP_CLOSE_PRECHARGE,
    STARTUP_HOLD_CLOSE_PRECHARGE,
    STARTUP_CLOSE_POS,
    STARTUP_HOLD_CLOSE_POS,
    STARTUP_OPEN_PRECHARGE,
    RUNNING,
    LOW_SOC,
    ERR_RUNNING
};

enum class DiCmd {
    INIT,
    HV_IS_ON,
    READY_TO_DRIVE,
    SHUTDOWN,
    RUN_ERROR,
    ERR_RESET,
};

using DiSts = generated::can::TxFC_Status::DiStatus_t;

enum class GovSts {
    INIT,
    STARTUP_HV,
    STARTUP_READY_TO_DRIVE,
    STARTUP_MOTOR,
    STARTUP_SEND_READY_TO_DRIVE,
    RUNNING,
    SHUTDOWN,
    ERR_STARTUP_HV,
    ERR_STARTUP_MOTOR_RESET,
    ERR_STARTUP_MOTOR_FAULT,
    ERR_STARTUP_DI,
    ERR_RUNNING_HV,
    ERR_RUNNING_MOTOR
};

enum class MiCmd {
    INIT,
    STARTUP,
    SHUTDOWN,
    ERR_RESET
};

enum class MiSts {
    INIT,
    STARTUP,
    READY,
    RUNNING,
    ERR,
    OFF
};
