#include <fcntl.h>
#include <termios.h>
#include <unistd.h>

#include <iostream>

#include "mcal/linux/periph/can.hpp"
#include "mcal/linux/periph/digital_input.hpp"

class KeyInput : public shared::periph::DigitalInput {
public:
    explicit KeyInput(char key) : key_(key) {
        // Set terminal to non-blocking mode
        termios t;
        tcgetattr(STDIN_FILENO, &t);
        original_ = t;         // Save original settings
        t.c_lflag &= ~ICANON;  // Disable canonical mode
        t.c_lflag &= ~ECHO;    // Disable echo
        tcsetattr(STDIN_FILENO, TCSANOW, &t);
        fcntl(STDIN_FILENO, F_SETFL, O_NONBLOCK);  // Set non-blocking mode
    }

    ~KeyInput() {
        // Restore terminal settings
        tcsetattr(STDIN_FILENO, TCSANOW, &original_);
    }

    bool Read() override {
        char input_key;
        if (read(STDIN_FILENO, &input_key, 1) > 0) {
            if (input_key == key_) {
                std::cout << "|" << key_ << "| is down" << std::endl;
                return true;
            }
        }
        return false;
    }

private:
    char key_;
    termios original_;  // Store original terminal settings
};

namespace mcal {
using namespace mcal::lnx::periph;

CanBase veh_can_base{"vcan0"};
KeyInput button_scroll{'\t'};
KeyInput button_select{'/'};

}  // namespace mcal

namespace bindings {

shared::periph::CanBase& veh_can_base = mcal::veh_can_base;
shared::periph::DigitalInput& button_scroll = mcal::button_scroll;
shared::periph::DigitalInput& button_select = mcal::button_select;

void Initialize() {
    std::cout << "Starting Linux Platform" << std::endl;
    mcal::veh_can_base.Setup();
}

void DelayMS(uint32_t ms) {
    usleep(ms * 1000);
}
}  // namespace bindings
