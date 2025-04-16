#ifndef STATE_MACHINE_H
#define STATE_MACHINE_H

// Define the enum for the state machine
typedef enum {
    STATE_LOGO_WAITING,
    STATE_DASHBOARD,   // Dashboard state
    STATE_DRIVE_MODE,  // Drive mode state
    STATE_DEBUG,       // Debug mode state
    STATE_PROFILES,    // Profiles state
    STATE_DRIVER,
    STATE_MODE,
    STATE_CONFIRM,
    STATE_WAITING,
    STATE_HV,
    STATE_MOTORS,
    STATE_START_DRIVING,
} dashboardStates;

#endif  // STATE_MACHINE_H