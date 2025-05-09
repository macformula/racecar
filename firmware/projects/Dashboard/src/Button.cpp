#include "inc/Button.hpp"

#include "../bindings.hpp"

Button::Button(shared::periph::DigitalInput& input) : input_(input) {
    previous_state_ = false;
    last_change_time_ = 0;
}

bool Button::PosEdge() const {
    return pos_edge_;
}

bool Button::NegEdge() const {
    return neg_edge_;
}

bool Button::IsPressed() const {
    return previous_state_;
}

uint32_t Button::GetHeldDuration() const {
    return last_update_time_ - last_change_time_;
}

void Button::Update(int time_ms) {
    bool new_state = input_.Read();
    if (new_state != previous_state_) {
        if (time_ms - last_change_time_ >= kDebounceDelay) {
            previous_state_ = new_state;

            last_change_time_ = time_ms;

            // based on the outer IF statement, we must be on an edge
            neg_edge_ = !new_state;
            pos_edge_ = new_state;
        }
    } else {
        neg_edge_ = false;
        pos_edge_ = false;
    }
    last_update_time_ = time_ms;
}
