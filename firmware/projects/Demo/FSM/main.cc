#include <conio.h>

#include <iostream>

#include "bindings.h"
#include "etl/fsm.h"
#include "etl/message_packet.h"
#include "etl/vector.h"

using namespace std;
const etl::message_router_id_t MOTOR_CONTROL = 0;

// Define your state identifiers
struct EventId {  // Initiates actions, transitions, or signals FSM, triggers
                  // events
    enum {
        START = 0,
        STOP = 1,
        STOPPED = 2,
    };
};

struct StateId {  // Defines actions and responses to events
    enum {
        IDLE = 0,
        RUNNING = 1,
        WINDING_DOWN = 2,
    };
};

// Event Messages
class Start : public etl::message<EventId::START> {
public:
    Start() {
        cout << "Start event triggered.\n";
    }
};

class Running : public etl::message<EventId::STOPPED> {
public:
    Running() {
        cout << "Running event triggered.\n";
    }
};

class Stop : public etl::message<EventId::STOP> {
public:
    Stop() {
        cout << "Stop event triggered.\n";
    }
};

// Base state class for all motor states
class MotorControl : public etl::fsm {
public:
    MotorControl() : fsm(MOTOR_CONTROL), current_state(StateId::IDLE) {}
    int current_state;

    virtual void on_enter() {}  // Virtual to allow overriding in derived states

    // Generic on_event method that can be specialized per event type
    virtual void on_event(const Start&) {}
    virtual void on_event(const Running&) {}
    virtual void on_event(const Stop&) {}
    void LogUnknownEvent(etl::imessage& msg) {
        cout << "Error, unknown event";
    }

    void transitionUp() {
        if (current_state == StateId::IDLE) {
            current_state = StateId::RUNNING;
        } else if (current_state == StateId::RUNNING) {
            current_state = StateId::WINDING_DOWN;
        }
        // If already in WINDING_DOWN, stay there
    }

    // Transition to the previous state (down arrow)
    void transitionDown() {
        if (current_state == StateId::WINDING_DOWN) {
            current_state = StateId::RUNNING;
        } else if (current_state == StateId::RUNNING) {
            current_state = StateId::IDLE;
        }
        // If already in IDLE, stay there
    }

    void handleInput() {
        while (true) {
            int ch = _getch();

            switch (ch) {
                case 'w':  // Up key (W)
                case 'W':  // Handle uppercase W as well
                    transitionUp();
                    break;
                case 's':  // Down key (S)
                case 'S':  // Handle uppercase S as well
                    transitionDown();
            }
        }
    }
};

class IdleState
    : public etl::fsm_state<MotorControl, IdleState, StateId::IDLE, Start> {
public:
    IdleState() {
        cout << "Initializing Stop State\n";
    }

    etl::fsm_state_id_t on_entry(const Stop& event) {
        get_fsm_context().current_state = StateId::IDLE;
        cout << "Now in Idle State\n";
        return STATE_ID;
    }

    etl::fsm_state_id_t on_exit(const Stop& event) {
        cout << "Exiting Idle State, transitioning to Running\n";
        return StateId::RUNNING;
    }

    etl::fsm_state_id_t on_event_unknown(
        etl::imessage& event) {  // an event represents an action or signal that
                                 // can trigger a change in the state of a
                                 // finite state machine (FSM).
        get_fsm_context().LogUnknownEvent(event);
        return STATE_ID;
    }
};

// Running State
class RunningState
    : public etl::fsm_state<MotorControl, Running, StateId::RUNNING, Stop> {
public:
    RunningState() {
        cout << "Initializing Running State\n";
    }

    etl::fsm_state_id_t on_entry(const Running& event) {
        get_fsm_context().current_state = StateId::RUNNING;
        cout << "Now in Running State\n";
        return STATE_ID;
    }

    etl::fsm_state_id_t on_exit(const Running& event) {
        cout << "Exiting Running State, transitioning to Winding Down\n";
        return StateId::WINDING_DOWN;
    }

    etl::fsm_state_id_t on_event_unknown(
        etl::imessage& event) {  // an event represents an action or signal that
                                 // can trigger a change in the state of a
                                 // finite state machine (FSM).
        get_fsm_context().LogUnknownEvent(event);
        return STATE_ID;
    }
};

// Winding Down State
class Winding_DownState : public etl::fsm_state<MotorControl, Winding_DownState,
                                                StateId::WINDING_DOWN, Stop> {
public:
    Winding_DownState() {
        cout << "Initializing Start State\n";
    }

    etl::fsm_state_id_t on_entry(const Start& event) {
        get_fsm_context().current_state = StateId::WINDING_DOWN;
        cout << "Now in Winding Down State\n";
        return STATE_ID;
    }

    etl::fsm_state_id_t on_exit(const Start& event) {
        cout << "Exiting Winding Down State, transitioning to Idle\n";
        return StateId::IDLE;
    }

    etl::fsm_state_id_t on_event_unknown(
        etl::imessage& event) {  // an event represents an action or signal that
                                 // can trigger a change in the state of a
                                 // finite state machine (FSM).
        get_fsm_context().LogUnknownEvent(event);
        return STATE_ID;
    }
};


int main() {
    // Instantiate the MotorControl FSM
    MotorControl motorControlFSM;

    // Instantiate each state and associate with the FSM
    IdleState idleState;
    RunningState runningState;
    Winding_DownState windingDownState;

    // while (true) {
    //     int ch = _getch();

    //     if (ch == 'w' || ch == 'W') {
    //         motorControlFSM.transitionUp();
    //     } else if (ch == 's' || ch == 'S') {
    //         motorControlFSM.transitionDown();
    //     } else if (ch == ' ') {
    //         break;
    //     }
    // }

    return 0;
}
