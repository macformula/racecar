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

  typedef etl::message_packet<Start, Stop, Stopped> message_packet;

  // The queue of message items.
  etl::queue<message_packet, 10> queue;

    // void transitionUp() {
    //     if (current_state == StateId::IDLE) {
    //         current_state = StateId::RUNNING;
    //     } else if (current_state == StateId::RUNNING) {
    //         current_state = StateId::WINDING_DOWN;
    //     }
    //     // If already in WINDING_DOWN, stay there
    // }

    // // Transition to the previous state (down arrow)
    // void transitionDown() {
    //     if (current_state == StateId::WINDING_DOWN) {
    //         current_state = StateId::RUNNING;
    //     } else if (current_state == StateId::RUNNING) {
    //         current_state = StateId::IDLE;
    //     }
    //     // If already in IDLE, stay there
    // }

    // void handleInput() {
    //     while (true) {
    //         int ch = _getch();

    //         switch (ch) {
    //             case 'w':  // Up key (W)
    //             case 'W':  // Handle uppercase W as well
    //                 transitionUp();
    //                 break;
    //             case 's':  // Down key (S)
    //             case 'S':  // Handle uppercase S as well
    //                 transitionDown();
    //         }
    //     }
    // }

    
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

    etl::fsm_state_id_t on_event_unknown(
        const etl::imessage& event) {  // an event represents an action or signal that
                                 // can trigger a change in the state of a
                                 // finite state machine (FSM).
        std::cout << "  S1 : Received unknown message " << int(event.get_message_id()) << std::endl;
        std::cout.flush();
        return StateId::IDLE;
    }

    // etl::fsm_state_id_t on_exit_state(const Start& Start){
    //     cout << "  S1 : Received message " << int(Start.get_message_id()) << " : '" << EventId::START << "'" << std::endl;
    //     cout.flush();
    //     return StateId::RUNNING;

    // }
};

// Running State
class RunningState
    : public etl::fsm_state<MotorControl, RunningState, StateId::RUNNING, Start, Stop, Stopped> {
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

        etl::fsm_state_id_t on_event(const Stopped& event){
        std::cout << "  S2 : Received incompatible message " << int(event.get_message_id()) << std::endl;
        cout.flush();
        return StateId::RUNNING;
    }

    etl::fsm_state_id_t on_event_unknown(
        etl::imessage& event) {  // an event represents an action or signal that
                                 // can trigger a change in the state of a
                                 // finite state machine (FSM).
        std::cout << "  S2 : Received unknown message " << int(event.get_message_id()) << std::endl;
        std::cout.flush();
        return StateId::RUNNING;
    }

    // etl::fsm_state_id_t on_exit_state(const Start& Stop){
    //     cout << "  S1 : Received message " << int(Stop.get_message_id()) << " : '" << EventId::STOP << "'" << std::endl;
    //     cout.flush();
    //     return StateId::WINDING_DOWN;

    // }

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
        std::cout << "  S2 : Received incompatible message " << int(event.get_message_id()) << std::endl;
        cout.flush();
        return StateId::WINDING_DOWN;
    }

    etl::fsm_state_id_t on_event(const Stop& event){
        cout << "  S2 : Received message " << int(event.get_message_id()) << " : '" << EventId::STOP << "'" << std::endl;
        cout.flush();
        return StateId::WINDING_DOWN;
    }

    etl::fsm_state_id_t on_event(const Stopped& event){
        cout << "  S2 : Received message " << int(event.get_message_id()) << " : '" << EventId::STOPPED << "'" << std::endl;
        cout.flush();
        return StateId::IDLE;
    }

    etl::fsm_state_id_t on_event_unknown(
        etl::imessage& event) {  // an event represents an action or signal that
                                 // can trigger a change in the state of a
                                 // finite state machine (FSM).
        std::cout << "  S1 : Received unknown message " << int(event.get_message_id()) << std::endl;
        std::cout.flush();
        return StateId::RUNNING;
    }

    // etl::fsm_state_id_t on_exit_state(const Start& Stop){
    //     cout << "  S1 : Received message " << int(Stop.get_message_id()) << " : '" << EventId::STOP << "'" << std::endl;
    //     cout.flush();
    //     return StateId::WINDING_DOWN;

    // }

};


int main() {
    // Instantiate the MotorControl FSM
    MotorControl motorControlFSM;

    // Instantiate each state and associate with the FSM
    IdleState idleState;
    RunningState runningState;
    Winding_DownState windingDownState;

    Start startMsg;          // Trigger start state
    Stopped stoppedMsg;      // Trigger stopped state
    Stop stopMsg;            // Trigger stop state

    etl::ifsm_state* state_list[] = { &idleState, &runningState, &windingDownState };
    motorControlFSM.set_states(state_list, 3);
    motorControlFSM.start();

    // Queue messages to control the FSM
    etl::send_message(motorControlFSM, startMsg);
    //etl::send_message(motorControlFSM, Message1(2));
    etl::send_message(motorControlFSM, stoppedMsg);
    //etl::send_message(motorControlFSM, Message2(3.4));
    etl::send_message(motorControlFSM, stopMsg);
    //etl::send_message(motorControlFSM, Message3("World"));
    //etl::send_message(motorControlFSM, Message4());

    std::cout << std::endl;

    // Process the queued messages
    motorControlFSM.process_queue();

    return 0;

}

