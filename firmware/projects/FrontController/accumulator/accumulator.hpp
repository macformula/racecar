#pragma once

#include "../enums.hpp"

enum class AccCmd {
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

namespace accumulator {

AccSts GetState(void);
ContactorCommands GetContactorCommand(void);

void SetPackSoc(float pack_soc);

void Init(void);
void Update_100Hz(AccCmd command, ContactorFeedbacks contactors);

}  // namespace accumulator
