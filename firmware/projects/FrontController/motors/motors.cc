#include "motors.hpp"

#include "alerts/alerts.hpp"
#include "amk/amk.hpp"
#include "amk/starter.hpp"
#include "generated/can/pt_bus.hpp"
#include "generated/can/pt_messages.hpp"
#include "generated/can/veh_bus.hpp"
#include "generated/can/veh_messages.hpp"
#include "physical.hpp"

using namespace generated::can;

namespace motors {

amk::Amk<RxInv1_ActualValues1, RxInv1_ActualValues2, TxInv1_Setpoints1>
    amk_left;

amk::Amk<RxInv2_ActualValues1, RxInv2_ActualValues2, TxInv2_Setpoints1>
    amk_right;

amk::Starter left_starter;
amk::Starter right_starter;
static uint8_t counter;

amk::StarterState GetLeftStarterState(void) {
    return left_starter.GetState();
}

amk::StarterState GetRightStarterState(void) {
    return right_starter.GetState();
}

static Command command;

static State state;
static uint32_t elapsed;

static bool inverter_enable;

// ---------- Modifiers ----------

void SetCommand(Command cmd) {
    command = cmd;
}

void SetLeftRequest(Request request) {
    amk_left.SetRequest(request);
}

void SetRightRequest(Request request) {
    amk_right.SetRequest(request);
}

// ---------- Accessors ----------

State GetState(void) {
    return state;
}

bool GetInverterEnable(void) {
    return inverter_enable;
}

TxInv1_Setpoints1 GetLeftSetpoints(void) {
    return amk_left.GetSetpoints();
}

TxInv2_Setpoints1 GetRightSetpoints(void) {
    return amk_right.GetSetpoints();
}

amk::State GetLeftState(void) {
    return amk_left.GetState();
}

amk::State GetRightState(void) {
    return amk_right.GetState();
}

float GetMph(void) {
    constexpr float PI = 3.1415926;
    constexpr float in_per_rev = PI * tuning::motor_diam_inch;
    constexpr float in_per_mile = 12 * 5280;
    constexpr float min_per_hr = 60;

    float avg_rpm =
        (std::abs(amk_left.GetRpm()) + std::abs(amk_right.GetRpm())) / 2.f;

    float motor_rpm = avg_rpm / tuning::gearbox_ratio;
    return (motor_rpm * in_per_rev * min_per_hr) / in_per_mile;
}

generated::can::TxFcCounters GetCounters(void) {
    return TxFcCounters{
        .motor = counter,
        .amk1 = amk_left.counter,
        .amk2 = amk_right.counter,
        .starter1 = left_starter.counter,
        .starter2 = right_starter.counter,
    };
}

// ---------- Behaviour ----------

void Init(void) {
    command = Command::OFF;
    state = State::IDLE;
    inverter_enable = false;

    amk_left.Init();
    amk_right.Init();
}

void Update_100Hz(PtBus& pt_can, VehBus& veh_can, amk::Request req_left,
                  amk::Request req_right) {
    using enum State;

    State new_state = state;
    bool new_inverter_en;

    amk::Command cmd_left;
    amk::Command cmd_right;

    switch (state) {
        case IDLE:
            cmd_left = amk::Command::OFF;
            cmd_right = amk::Command::OFF;
            new_inverter_en = false;

            if (command == Command::ENABLED) {
                new_state = STARTING;
                left_starter.Reset();
                right_starter.Reset();
            }

            break;

        case STARTING:
            cmd_left = left_starter.Update(amk_left.GetState());
            cmd_right = right_starter.Update(amk_right.GetState());

            new_inverter_en = false;

            if (left_starter.HasErroredOut()) {
                alerts::Get().left_motor_starting_error = true;
                new_state = ERROR;
            }

            if (right_starter.HasErroredOut()) {
                alerts::Get().right_motor_starting_error = true;
                new_state = ERROR;
            }

            if (left_starter.Success() && right_starter.Success()) {
                new_state = SWITCHING_INVERTER_ON;
            }
            break;

        case SWITCHING_INVERTER_ON: {
            cmd_left = amk::Command::ENABLED;
            cmd_right = amk::Command::ENABLED;
            new_inverter_en = true;

            // Don't proceed until LV confirms the switch
            auto lv_msg = veh_can.GetRxLvStatus();

            if (lv_msg.has_value() && lv_msg->MotorControllerSwitchClosed()) {
                amk_left.ProceedFromInverter();
                amk_right.ProceedFromInverter();
                new_state = RUNNING;
            }

        } break;

        case RUNNING:
            cmd_left = amk::Command::ENABLED;
            cmd_right = amk::Command::ENABLED;
            new_inverter_en = true;

            if (amk_left.HasError()) {
                alerts::Get().left_motor_running_error = true;
                new_state = ERROR;
            }
            if (amk_right.HasError()) {
                alerts::Get().right_motor_running_error = true;
                new_state = ERROR;
            }
            break;

        case ERROR:
            cmd_left = amk::Command::OFF;
            cmd_right = amk::Command::OFF;
            new_inverter_en = false;
            break;
    }

    if (command == Command::OFF) {
        new_state = IDLE;
    }

    inverter_enable = new_inverter_en;

    amk_left.SetCommand(cmd_left);
    amk_left.SetRequest(req_left);

    amk_right.SetCommand(cmd_right);
    amk_right.SetRequest(req_right);

    auto l_av1 = pt_can.GetRxInv1_ActualValues1();
    auto l_av2 = pt_can.GetRxInv1_ActualValues2();
    if (l_av1.has_value() && l_av2.has_value()) {
        amk_left.Update_100Hz(l_av1.value(), l_av2.value());
        alerts::Get().no_inv1_can = false;
    } else {
        alerts::Get().no_inv1_can = true;
    }

    auto r_av1 = pt_can.GetRxInv2_ActualValues1();
    auto r_av2 = pt_can.GetRxInv2_ActualValues2();
    if (r_av1.has_value() && r_av2.has_value()) {
        amk_right.Update_100Hz(r_av1.value(), r_av2.value());
        alerts::Get().no_inv2_can = false;
    } else {
        alerts::Get().no_inv2_can = true;
    }

    if (new_state != state) {
        state = new_state;
        elapsed = 0;
    } else {
        elapsed += 10;
    }
    counter++;
}

}  // namespace motors
