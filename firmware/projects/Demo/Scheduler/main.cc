#include <iostream>

#include "bindings.hpp"
#include "scheduler/scheduler.h"

void t1(void) {
    std::cout << "Task 1" << std::endl;
}

void t2(void) {
    std::cout << "       Task 2" << std::endl;
}

int main() {
    scheduler_register_task(t1, 1000);
    scheduler_register_task(t2, 1400);

    scheduler_run();

    while (true) continue;  // never hit this, scheduler_run is infinite

    return 0;
}