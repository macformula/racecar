#include <conio.h>
#include "bindings.h"
#include "etl/queue.h"
#include "etl/fsm.h"
#include "etl/message_packet.h"
#include <iostream>
#include <string>

using namespace std;
// Define your state identifiers
struct EventId {  // Initiates actions, transitions, or signals FSM, triggers
                  // events
    enum {
        START = 0,
        STOP = 1,
        STOPPED = 2,
        SWITCH_D1 = 3,
        SWITCH_D2 = 4,
        SWITCH_D3 = 5,
    };
};

struct StateId {  // Defines actions and responses to events
    enum {
        IDLE = 0,
        RUNNING = 1,
        WINDING_DOWN = 2,
        GEAR_D1 = 3,
        GEAR_D2 = 4,
        GEAR_D3 = 5,
    };
};

// Event Messages
class Start : public etl::message<EventId::START> {
public:
    Start() {
    }
};

class Stopped : public etl::message<EventId::STOPPED> {
public:
    Stopped() {
    }
};

class Stop : public etl::message<EventId::STOP> {
public:
    Stop() {
    }
};

class Switch_D1 : public etl::message<EventId::SWITCH_D1> {
public:
    Switch_D1() {}
};

class Switch_D2 : public etl::message<EventId::SWITCH_D2> {
public:
    Switch_D2() {}
};

class Switch_D3 : public etl::message<EventId::SWITCH_D3> {
public:
    Switch_D3() {}
};

// Base state class for all motor states
class MotorControl : public etl::fsm {
public:

    MotorControl() : fsm(1){} // constructor

    void LogUnknownEvent(etl::imessage& msg) {
        cout << "Error, unknown event";
    }

    void receive(const etl::imessage& msg_) override{
    if (accepts(msg_))
    {
      // Place in queue.
      queue.emplace(msg_);

      std::cout << "Queueing message " << int(msg_.get_message_id()) << std::endl;
    }
    else
    {
      std::cout << "Ignoring message " << int(msg_.get_message_id()) << std::endl;
    }
  }

  //***************************************************************************
  // The method to call to handle any queued messages.
  //***************************************************************************
  void process_queue()
  {
    while (!queue.empty())
    {
      message_packet& packet = queue.front();
      etl::imessage& msg = packet.get();
      std::cout << "Processing message " << int(msg.get_message_id()) << std::endl;

      // Call the base class's receive function.
      // This will route it to the correct 'on_event' handler.
      etl::fsm::receive(msg);

      queue.pop();
    }
  }

private:
    typedef etl::message_packet<Start, Stop, Stopped, Switch_D1, Switch_D2,
                                Switch_D3>
        message_packet;

    // The queue of message items.
    etl::queue<message_packet, 10> queue;
};

class IdleState
    : public etl::fsm_state<MotorControl, IdleState, StateId::IDLE, Start, Stop, Stopped> {
public:

    etl::fsm_state_id_t on_enter_state() override {
        cout << "Now in Idle State\n";
        return StateId::IDLE;
    }

    void on_exit_state() override {
        cout << "Exiting Idle State, transitioning to Running\n";
    }

    etl::fsm_state_id_t on_event(const Stopped& event){
        cout << "  S1 : Received message " << int(event.get_message_id()) << " : '" << EventId::STOPPED << "'" << std::endl;
        cout.flush();
        return StateId::IDLE;
    }

    etl::fsm_state_id_t on_event(const Start& event){
        cout << "  S1 : Received message " << int(event.get_message_id()) << " : '" << EventId::START << "'" << std::endl;
        cout.flush();
        return StateId::RUNNING;
    }

    etl::fsm_state_id_t on_event(const Stop& event){
        std::cout << "  S1 : Received incompatible message " << int(event.get_message_id()) << std::endl;
        cout.flush();
        return StateId::IDLE;
    }

    etl::fsm_state_id_t on_event_unknown(const etl::imessage& event) {
        std::cout << "  S1 : Received unknown message " << int(event.get_message_id()) << std::endl;
        std::cout.flush();
        return StateId::IDLE;
    }
};

// Running State
class RunningState
    : public etl::fsm_state<MotorControl, RunningState, StateId::RUNNING, Start,
                            Stop, Stopped, Switch_D1, Switch_D2, Switch_D3> {
public:
    etl::fsm_state_id_t on_enter_state() override {
        cout << "Now in Running State\n";
        return StateId::RUNNING;
    }

    void on_exit_state() override {
        cout << "Exiting Running State, transitioning to Winding Down\n";
    }

    etl::fsm_state_id_t on_event(const Stop& event){
        cout << "  S2 : Received message " << int(event.get_message_id()) << " : '" << EventId::STOP << "'" << std::endl;
        cout.flush();
        return StateId::WINDING_DOWN;
    }

    etl::fsm_state_id_t on_event(const Start& event){
        cout << "  S2 : Received message " << int(event.get_message_id()) << " : '" << EventId::START << "'" << std::endl;
        cout.flush();
        return StateId::RUNNING;
    }

    etl::fsm_state_id_t on_event(const Stopped& event) {
        std::cout << "  S2 : Received incompatible message " << int(event.get_message_id()) << std::endl;
        cout.flush();
        return StateId::RUNNING;
    }

    etl::fsm_state_id_t on_event(const Switch_D1& event) {
        std::cout << "  S2 : Received message " << int(event.get_message_id())
                  << std::endl;
        cout.flush();
        return StateId::GEAR_D1;
    }

    etl::fsm_state_id_t on_event(const Switch_D2& event) {
        std::cout << "  S2 : Received incompatible message "
                  << int(event.get_message_id()) << std::endl;
        cout.flush();
        return StateId::RUNNING;
    }

    etl::fsm_state_id_t on_event(const Switch_D3& event) {
        std::cout << "  S2 : Received incompatible message "
                  << int(event.get_message_id()) << std::endl;
        cout.flush();
        return StateId::RUNNING;
    }

    etl::fsm_state_id_t on_event_unknown(const etl::imessage& event) {
        std::cout << "  S2 : Received unknown message " << int(event.get_message_id()) << std::endl;
        std::cout.flush();
        return StateId::RUNNING;
    }
};

class GEAR_D1
    : public etl::fsm_state<MotorControl, RunningState, StateId::RUNNING, Start,
                            Stop, Stopped, Switch_D1, Switch_D2, Switch_D3> {
    etl::fsm_state_id_t on_event(const Start& event) {
        std::cout << "  SS1 : Received incompatible message "
                  << int(event.get_message_id()) << std::endl;
        cout.flush();
        return StateId::GEAR_D1;
    }

    etl::fsm_state_id_t on_event(const Stop& event) {
        std::cout << "  SS1 : Received message " << int(event.get_message_id())
                  << std::endl;
        cout.flush();
        return StateId::WINDING_DOWN;
    }

    etl::fsm_state_id_t on_event(const Stopped& event) {
        std::cout << "  SS1 : Received incompatible message "
                  << int(event.get_message_id()) << std::endl;
        cout.flush();
        return StateId::GEAR_D1;
    }

    etl::fsm_state_id_t on_event(const Switch_D1& event) {
        cout.flush();
        return StateId::GEAR_D1;
    }

    etl::fsm_state_id_t on_event(const Switch_D2& event) {
        std::cout << "  SS1 : Received message " << int(event.get_message_id())
                  << std::endl;
        cout.flush();
        return StateId::GEAR_D2;
    }

    etl::fsm_state_id_t on_event(const Switch_D3& event) {
        std::cout
            << "  SS1 : Received incompatible message "
            << int(event.get_message_id())
            << std::endl;  // Can't go from D1 to D3, has to be D1 -> D2 -> D3
        cout.flush();
        return StateId::GEAR_D1;
    }

    etl::fsm_state_id_t on_event_unknown(const etl::imessage& event) {
        std::cout << "  SS1 : Received unknown message "
                  << int(event.get_message_id()) << std::endl;
        std::cout.flush();
        return StateId::GEAR_D1;
    }
};

class GEAR_D2
    : public etl::fsm_state<MotorControl, RunningState, StateId::RUNNING, Start,
                            Stop, Stopped, Switch_D1, Switch_D2, Switch_D3> {
    etl::fsm_state_id_t on_event(const Start& event) {
        std::cout << "  SS2 : Received incompatible message "
                  << int(event.get_message_id()) << std::endl;
        cout.flush();
        return StateId::GEAR_D2;
    }

    etl::fsm_state_id_t on_event(const Stop& event) {
        std::cout << "  SS2 : Received message " << int(event.get_message_id())
                  << std::endl;
        cout.flush();
        return StateId::WINDING_DOWN;
    }

    etl::fsm_state_id_t on_event(const Stopped& event) {
        std::cout << "  SS2 : Received incompatible message "
                  << int(event.get_message_id()) << std::endl;
        cout.flush();
        return StateId::GEAR_D2;
    }

    etl::fsm_state_id_t on_event(const Switch_D1& event) {
        std::cout << "  SS2 : Received message " << int(event.get_message_id())
                  << std::endl;
        cout.flush();
        return StateId::GEAR_D1;
    }

    etl::fsm_state_id_t on_event(const Switch_D2& event) {
        std::cout << "  SS2 : Received message " << int(event.get_message_id())
                  << std::endl;
        cout.flush();
        return StateId::GEAR_D2;
    }

    etl::fsm_state_id_t on_event(const Switch_D3& event) {
        std::cout << "  SS2 : Received message " << int(event.get_message_id())
                  << std::endl;
        cout.flush();
        return StateId::GEAR_D3;
    }

    etl::fsm_state_id_t on_event_unknown(const etl::imessage& event) {
        std::cout << "  SS2 : Received unknown message "
                  << int(event.get_message_id()) << std::endl;
        std::cout.flush();
        return StateId::GEAR_D2;
    }
};

class GEAR_D3
    : public etl::fsm_state<MotorControl, RunningState, StateId::RUNNING, Start,
                            Stop, Stopped, Switch_D1, Switch_D2, Switch_D3> {
    etl::fsm_state_id_t on_event(const Start& event) {
        std::cout << "  SS3 : Received incompatible message "
                  << int(event.get_message_id()) << std::endl;
        cout.flush();
        return StateId::GEAR_D3;
    }

    etl::fsm_state_id_t on_event(const Stop& event) {
        std::cout << "  SS3 : Received message " << int(event.get_message_id())
                  << std::endl;
        cout.flush();
        return StateId::WINDING_DOWN;
    }

    etl::fsm_state_id_t on_event(const Stopped& event) {
        std::cout << "  SS3 : Received incompatible message "
                  << int(event.get_message_id()) << std::endl;
        cout.flush();
        return StateId::GEAR_D3;
    }

    etl::fsm_state_id_t on_event(const Switch_D1& event) {
        std::cout << "  SS3 : Received incompatible message "
                  << int(event.get_message_id()) << std::endl;
        cout.flush();
        return StateId::GEAR_D3;
    }

    etl::fsm_state_id_t on_event(const Switch_D2& event) {
        std::cout << "  SS3 : Received message " << int(event.get_message_id())
                  << std::endl;
        cout.flush();
        return StateId::GEAR_D2;
    }

    etl::fsm_state_id_t on_event(const Switch_D3& event) {
        std::cout << "  SS3 : Received message " << int(event.get_message_id())
                  << std::endl;
        cout.flush();
        return StateId::GEAR_D3;
    }

    etl::fsm_state_id_t on_event_unknown(const etl::imessage& event) {
        std::cout << "  SS3 : Received unknown message "
                  << int(event.get_message_id()) << std::endl;
        std::cout.flush();
        return StateId::GEAR_D3;
    }
};

// Winding Down State
class Winding_DownState : public etl::fsm_state<MotorControl, Winding_DownState, StateId::WINDING_DOWN, Start, Stop, Stopped> {
public:
        etl::fsm_state_id_t on_enter_state() override {
        cout << "Now in Winding Down State\n";
        return StateId::WINDING_DOWN;
    }

    void on_exit_state() override {
        cout << "Exiting Winding Down State, transitioning to Idle\n";
    }

    etl::fsm_state_id_t on_event(const Start& event){
        std::cout << "  S3 : Received incompatible message "
                  << int(event.get_message_id()) << std::endl;
        cout.flush();
        return StateId::WINDING_DOWN;
    }

    etl::fsm_state_id_t on_event(const Stop& event){
        cout << "  S3 : Received message " << int(event.get_message_id())
             << " : '" << EventId::STOP << "'" << std::endl;
        cout.flush();
        return StateId::WINDING_DOWN;
    }

    etl::fsm_state_id_t on_event(const Stopped& event){
        cout << "  S3 : Received message " << int(event.get_message_id())
             << " : '" << EventId::STOPPED << "'" << std::endl;
        cout.flush();
        return StateId::IDLE;
    }

    etl::fsm_state_id_t on_event_unknown(const etl::imessage& event) {
        std::cout << "  S3 : Received unknown message "
                  << int(event.get_message_id()) << std::endl;
        std::cout.flush();
        return StateId::RUNNING;
    }
};

void handleInput(MotorControl& motorControlFSM) {
    while (true) {
        int ch = _getch();  // Capture key press

        switch (ch) {
            case 'w':  // 'w' or 'W' key to send Start event
            case 'W': {
                Start startMsg;
                etl::send_message(motorControlFSM, startMsg);
                std::cout << "Sent Start message\n";
            } break;
            case 's':  // 's' or 'S' key to send Stop event
            case 'S': {
                Stop stopMsg;
                etl::send_message(motorControlFSM, stopMsg);
                std::cout << "Sent Stop message\n";
            } break;
            case 'd':  // 'd' or 'D' to go into hfsm
            case 'D': {
                Switch_D1 gearD1;
                etl::send_message(motorControlFSM, gearD1);
                std::cout << "Sent D1 Gear Switch message\n";
            } break;
            case 'e':  //
            case 'E': {
                Switch_D2 gearD2;
                etl::send_message(motorControlFSM, gearD2);
                std::cout << "Sent D2 Gear Switch message\n";
            } break;
            case 'f':  //
            case 'F': {
                Switch_D3 gearD3;
                etl::send_message(motorControlFSM, gearD3);
                std::cout << "Sent D3 Gear Switch message\n";
            } break;
            case 'a':  // 'x' key to send Stopped event
            case 'A': {
                Stopped stoppedMsg;
                etl::send_message(motorControlFSM, stoppedMsg);
                std::cout << "Sent Stopped message\n";
            } break;
            case 'q':  // 'q' to quit the loop and stop the FSM
            case 'Q':
                std::cout << "Exiting...\n";
                return;
            default:
                std::cout << "Unknown key pressed\n";
                break;
        }

        // Process the queued messages after each key press
        motorControlFSM.process_queue();
    }
}

int main() {
    // Instantiate the MotorControl FSM
    MotorControl motorControlFSM;

    // Instantiate each state and associate with the FSM
    IdleState idleState;
    RunningState runningState;
    Winding_DownState windingDownState;

    GEAR_D1 gearD1;
    GEAR_D2 gearD2;
    GEAR_D3 gearD3;

    Start startMsg;          // Trigger start state
    Stopped stoppedMsg;      // Trigger stopped state
    Stop stopMsg;            // Trigger stop state

    etl::ifsm_state* state_list[] = {&idleState,        &runningState,
                                     &windingDownState, &gearD1,
                                     &gearD2,           &gearD3};

    etl::ifsm_state* childStates[] = {&gearD1, &gearD2, &gearD3};

    motorControlFSM.set_states(state_list, 6);
    runningState.set_child_states(childStates, 3);
    motorControlFSM.start();

    // Queue messages to control the FSM
    handleInput(motorControlFSM);

    std::cout << std::endl;

    // Process the queued messages
    motorControlFSM.process_queue();

    return 0;
}

