#pragma once

#include "generated/can/veh_messages.hpp"

using DiSts = generated::can::TxFC_Status::DiStatus_t;
using GovSts = generated::can::TxFC_Status::GovStatus_t;
using BmSts = generated::can::TxFC_Status::BmStatus_t;
using MiSts = generated::can::TxFC_Status::MiStatus_t;

enum class BmCmd {
    INIT,
    STARTUP,
};

enum class DiCmd {
    INIT,
    HV_IS_ON,
    READY_TO_DRIVE,
    SHUTDOWN,
    RUN_ERROR,
    ERR_RESET,
};

enum class MiCmd {
    INIT,
    STARTUP,
    SHUTDOWN,
    ERR_RESET
};
