#include "battery_monitor.h"

BatteryMonitor::BatteryMonitor() {
    // put initialization logic here (if you need any)
}

BmOutput BatteryMonitor::update(const BmInput& input, int time_ms) {
    BmOutput output;

    // put logic here

    // you will need to implement a state machine of some form. For now, I
    // recommend using a switch-case state machine. google for some examples

    return output;
}