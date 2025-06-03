#include "scheduler.h"

#include <etl/vector.h>

#include <cstdint>

#include "../bindings.hpp"

static const uint32_t SCHEDULER_MAX_TASKS = 16;

struct task_t {
    uint32_t period_ms;
    uint32_t last_run_ms;
    void (*run)(void*);
    void* argument;
};

static etl::vector<task_t, SCHEDULER_MAX_TASKS> tasks;

void scheduler_register_task(void (*task)(void*), uint32_t period_ms,
                             void* argument) {
    task_t new_task = {
        .period_ms = period_ms,
        .last_run_ms = 0,
        .run = task,
        .argument = argument,
    };

    tasks.push_back(new_task);
}

void scheduler_run(void) {
    while (true) {
        for (auto& task : tasks) {
            uint32_t current_time = bindings::GetTickMs();
            if (current_time - task.last_run_ms >= task.period_ms) {
                task.run(task.argument);
                task.last_run_ms = current_time;
            }
        }
    }
}