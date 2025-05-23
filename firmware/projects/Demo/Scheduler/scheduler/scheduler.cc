#include "scheduler.h"

#include <etl/vector.h>

#include <cstdint>

#include "../bindings.hpp"

static const uint32_t SCHEDULER_MAX_TASKS = 10;

struct task_t {
    uint32_t period;    // in ms
    uint32_t last_run;  // in ms
    void (*task)(void);
};

static etl::vector<task_t, SCHEDULER_MAX_TASKS> tasks;

void scheduler_register_task(void (*task)(void), uint32_t period_ms) {
    task_t new_task = {
        .period = period_ms,
        .last_run = 0,
        .task = task,
    };

    tasks.push_back(new_task);
}

void scheduler_run(void) {
    while (true) {
        uint32_t current_time = bindings::GetTickMs();

        for (auto& task : tasks) {
            if (current_time - task.last_run >= task.period) {
                task.task();
                task.last_run = current_time;
            }
        }
    }
}