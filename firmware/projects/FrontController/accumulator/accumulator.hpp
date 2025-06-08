#pragma once

#include "generated/can/veh_bus.hpp"
#include "generated/can/veh_messages.hpp"

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

namespace accumulator {

using State = generated::can::TxFC_Status::AccStatus_t;

enum class Command {
    OFF,
    ENABLED,
};

State GetState(void);
generated::can::TxContactorCommand GetContactorCommand(void);
generated::can::TxAccumulator_Soc GetDebugMsg(void);
float GetSocPercent(void);
float GetPrechargePercent(void);

void SetCommand(Command command);

void Init(void);
void Update_100Hz(generated::can::VehBus& veh_can);

}  // namespace accumulator
