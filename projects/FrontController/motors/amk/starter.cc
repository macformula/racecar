#include "starter.hpp"

namespace amk {

void Starter::Reset(void) {
    current_attempt = 0;
    state = StarterState::OFF;
}

StarterState Starter::GetState(void) const {
    return state;
}

bool Starter::HasErroredOut(void) const {
    return state == StarterState::FAILED;
}

bool Starter::Success(void) const {
    return state == StarterState::SUCCESS;
}

Command Starter::Update(State amk_state) {
    StarterState new_state = state;
    Command command;

    switch (state) {
        case StarterState::OFF:
            command = Command::ENABLED;

            if (amk_state == State::ERROR) {
                // don't increment counter here. only want to count errors after
                // system ready
                new_state = StarterState::ERROR_DETECTED;
            }

            if (amk_state == State::SYSTEM_READY) {
                new_state = StarterState::STARTING;
            }

            break;

        case StarterState::STARTING:
            command = Command::ENABLED;

            if (amk_state == State::ERROR) {
                if (current_attempt >= RETRY_ATTEMPTS) {
                    new_state = StarterState::FAILED;
                } else {
                    new_state = StarterState::ERROR_DETECTED;
                }
            }

            if (amk_state == State::STARTUP_X140) {
                new_state = StarterState::SUCCESS;
            }
            break;

        case StarterState::SUCCESS:
            command = Command::ENABLED;
            break;

        case StarterState::FAILED:
            command = Command::OFF;
            break;

        case StarterState::ERROR_DETECTED:
            command = Command::ERROR_RESET;

            if (amk_state == State::ERROR_RESET) {
                new_state = StarterState::RESET_ERROR;
            }

            break;

        case StarterState::RESET_ERROR:
            command = Command::OFF;
            if (amk_state == State::OFF) {
                current_attempt++;

                new_state = StarterState::OFF;
            }

            break;
    }

    state = new_state;
    counter++;
    return command;
}

}  // namespace amk
