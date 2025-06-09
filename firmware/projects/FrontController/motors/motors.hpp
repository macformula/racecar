#pragma once

#include "amk/amk.hpp"
#include "generated/can/pt_messages.hpp"
#include "generated/can/veh_messages.hpp"

namespace motors {

using Request = amk::Request;
using State = generated::can::TxFcStatus::MotorState_t;

enum class Command {
    OFF,
    ENABLED
};

// ---------- Modifiers ----------
void SetCommand(Command cmd);
void SetLeftRequest(Request request);
void SetRightRequest(Request request);

// ---------- Accessors ----------
State GetState(void);
bool GetInverterEnable(void);

amk::State GetLeftState(void);
amk::State GetRightState(void);

generated::can::TxInv1_Setpoints1 GetLeftSetpoints(void);
generated::can::TxInv2_Setpoints1 GetRightSetpoints(void);

// ---------- Behaviour ----------
void Init(void);
void Update_100Hz(generated::can::PtBus& pt_can,
                  generated::can::VehBus& veh_can, amk::Request req_left,
                  amk::Request req_right);

}  // namespace motors