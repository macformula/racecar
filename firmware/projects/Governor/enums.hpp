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
    ERR_RUN,
    ERR_RESET
};

enum class DiSts {
    UNKNOWN,
    INIT,
    IDLE,
    STARTUP,
    WAITING_FOR_DRIVER,
    READY_TO_DRIVE_REQ,
    HV_START_REQ,
    RUNNING,
    ERR
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
    ERR_STARTUP_RESET,
    ERR_STARTUP_MOTOR,
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