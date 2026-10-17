#pragma once

#include "generated/can/veh_bus.hpp"
#include "generated/can/veh_messages.hpp"
#include "moving_average.hpp"
#include "periph/analog_input.hpp"

namespace macfe::fc {

class Accumulator {
public:
    using State = generated::can::TxFcStatus::AccumulatorState_t;
    enum class Command {
        OFF,
        ENABLED,
    };
    enum class ContactorFeedback : bool {
        // WARNING: feedback is inverted relative to command
        IS_OPEN = true,
        IS_CLOSED = false,
    };

    enum class ContactorCommand {
        OPEN = false,
        CLOSE = true,
    };

    struct ContactorFeedbacks {
        ContactorFeedback precharge;
        ContactorFeedback negative;
        ContactorFeedback positive;
    };

    struct ContactorCommands {
        ContactorCommand precharge;
        ContactorCommand positive;
        ContactorCommand negative;
    };

    Accumulator(macfe::periph::AnalogInput& precharge_monitor)
        : _precharge_monitor(precharge_monitor) {
        state = State::IDLE;
        contactor_command = {
            .precharge = ContactorCommand::OPEN,
            .positive = ContactorCommand::OPEN,
            .negative = ContactorCommand::OPEN,
        };
        elapsed = 0;
        max_pack_voltage = 0;
        precharge_voltage = 0;
        pack_voltage = 0;
        pack_voltage_valid = false;
        pack_voltage_timeout = 0;
    }

    State GetState(void);

    generated::can::TxContactorCommand GetContactorCommand(void);
    generated::can::TxAccumulator_Soc GetDebugMsg(void);
    float GetSocPercent(void);
    float GetPrechargePercent(void);

    void SetCommand(Command command);

    void Update_100Hz(generated::can::VehBus& veh_can);

private:
    void MeasureVoltages(generated::can::VehBus& veh_can);
    bool IsPrechargeComplete();
    void UpdateStateMachine(ContactorFeedbacks fb);
    static bool FeedbackMatchesCommand(ContactorCommands cmd,
                                       ContactorFeedbacks fb);
    static std::optional<ContactorFeedbacks> ReadContactorFeedbacks(
        generated::can::VehBus& veh_can);
    macfe::periph::AnalogInput& _precharge_monitor;
    State state = State::IDLE;
    ContactorCommands contactor_command{};
    Command command = Command::OFF;
    uint32_t elapsed = 0;
    float pack_voltage = 0;
    uint32_t pack_voltage_timeout = 0;
    bool pack_voltage_valid = false;
    float precharge_voltage = 0;
    float max_pack_voltage = 0;
    macfe::MovingAverage<50> ma;
};

}  // namespace macfe::fc
