#pragma once

enum class BmCmd {
    INIT,
    HV_STARTUP,
    HV_SHUTDOWN
};

enum class BmSts {
    UNKNOWN,
    INIT,
    IDLE,
    STARTUP,
    INIT_PRECHARGE,
    PRECHARGE,
    RUNNING,
    HVIL_INTERRUPT,
    LOW_SOC,
    ERR_RUNNING
};

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
    IDLE,
    STARTUP,
    SHUTDOWN,
    ERR_RESET
};

enum class MiSts {
    UNKNOWN,
    INIT,
    STARTUP,
    READY,
    RUNNING,
    SHUTDOWN,
    ERR,
    OFF
};