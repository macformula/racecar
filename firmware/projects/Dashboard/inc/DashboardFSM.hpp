#ifndef STATE_MACHINE_H
#define STATE_MACHINE_H

// Define the enum for the state machine
typedef enum {
    STATE_DASHBOARD,   //Dashboard state
    STATE_DRIVE_MODE,  //Drive mode state
    STATE_DEBUG,       //Debug mode state
    STATE_PROFILES,    //Profiles state
} dashboardStates;

#endif // STATE_MACHINE_H