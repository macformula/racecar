#include <conio.h>  // For _getch

#include <iostream>

#include "fsm_library.h"  // Replace with the actual header file name

int main() {
    // Instantiate the MotorControl FSM
    MotorControl motorControlFSM;

    // Instantiate each state and associate with the FSM
    IdleState idleState;
    RunningState runningState;
    Winding_DownState windingDownState;

    while (true) {
        int ch = _getch();

        if (ch == 'w' || ch == 'W') {
            motorControlFSM.transitionUp();
        } else if (ch == 's' || ch == 'S') {
            motorControlFSM.transitionDown();
        } else if (ch == ' ') {
            break;
        }
    }

    return 0;
}
