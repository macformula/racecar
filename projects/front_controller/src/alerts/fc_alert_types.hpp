
#pragma once

#include <cstdint>

namespace alerts {

enum class FcAlert : uint32_t {
    AppsImplausible,
    AccumulatorLowSoc,
    AccumulatorContactorWrongState,
    MotorRetriesExceeded,
    LeftMotorStartingError,
    RightMotorStartingError,
    LeftMotorRunningError,
    RightMotorRunningError,
    DashboardBootTimeout,
    CanTxError,
    EV47Active,
    NoInv1Can,
    NoInv2Can,
};

}  // namespace alerts
