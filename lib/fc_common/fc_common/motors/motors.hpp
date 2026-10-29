#pragma once

#include "amk/amk.hpp"
#include "amk/starter.hpp"
#include "generated/can/pt_bus.hpp"
#include "generated/can/pt_messages.hpp"
#include "generated/can/veh_bus.hpp"
#include "generated/can/veh_messages.hpp"

namespace macfe::fc {

class Motors {
public:
    enum class Command {
        OFF,
        ENABLED
    };
    Motors() {
        command = Command::OFF;
        state = State::IDLE;
        inverter_enable = false;

        amk_left.Init();
        amk_right.Init();
    }

    using Request = amk::Request;
    using State = generated::can::TxFcStatus::MotorState_t;

    // ---------- Modifiers ----------
    void SetCommand(Command cmd);
    void SetLeftRequest(Request request);
    void SetRightRequest(Request request);

    // ---------- Accessors ----------
    State GetState(void);
    bool GetInverterEnable(void);
    float GetMph(void);

    amk::State GetLeftState(void);
    amk::State GetRightState(void);

    generated::can::TxInv1_Setpoints1 GetLeftSetpoints(void);
    generated::can::TxInv2_Setpoints1 GetRightSetpoints(void);

    amk::StarterState GetLeftStarterState(void);
    amk::StarterState GetRightStarterState(void);

    generated::can::TxFcCounters GetCounters(void);

    // ---------- Behaviour ----------
    void Update_100Hz(generated::can::PtBus& pt_can,
                      generated::can::VehBus& veh_can, amk::Request req_left,
                      amk::Request req_right);

private:
    amk::Amk<generated::can::RxInv1_ActualValues1,
             generated::can::RxInv1_ActualValues2,
             generated::can::TxInv1_Setpoints1>
        amk_left;
    amk::Amk<generated::can::RxInv2_ActualValues1,
             generated::can::RxInv2_ActualValues2,
             generated::can::TxInv2_Setpoints1>
        amk_right;
    amk::Starter left_starter{};
    amk::Starter right_starter{};
    uint8_t counter = 0;
    Command command = Command::OFF;
    State state = State::IDLE;
    uint32_t elapsed = 0;
    bool inverter_enable = false;
};

}  // namespace macfe::fc