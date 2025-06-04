#pragma once

#include "../accumulator/accumulator.hpp"
#include "../driver_interface/driver_interface.hpp"
#include "../enums.hpp"
#include "../motors/motor_interface.hpp"

namespace governor {

AccCmd GetAccumulatorCmd(void);
MiCmd GetMotorCmd(void);
DiCmd GetDriverInterfaceCmd(void);
GovSts GetState(void);

void Init(void);
void Update_100Hz(AccSts acc, MiSts mi, DiSts di);

}  // namespace governor
