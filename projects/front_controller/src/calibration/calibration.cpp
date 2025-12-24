#include "calibration.hpp"

#include "sensors/driver/driver.hpp"

namespace calibration {

using namespace generated::can;
//---static variables---
static State state = State::IDLE;
static Command command = Command::STOP;
static uint32_t elapsed = 0;

//---modifiers---
void SetCommand(Command cmd) {
    command = cmd;
}

}  // namespace calibration