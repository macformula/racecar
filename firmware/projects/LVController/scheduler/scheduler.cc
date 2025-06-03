#include "scheduler.hpp"

#include <etl/vector.h>

#include <cstdint>

#include "bindings.hpp"

static const uint32_t SCHEDULER_MAX_TASKS = 10;

struct task_t {
    uint32_t period_ms;
    uint32_t last_run_ms;
    void (*task)(void);
};

static etl::vector<task_t, SCHEDULER_MAX_TASKS> tasks;

namespace scheduler {

void register_task(void (*task)(void), uint32_t period_ms) {
    task_t new_task = {
        .period_ms = period_ms,
        .last_run_ms = 0,
        .task = task,
    };

    tasks.push_back(new_task);
}

void run(void) {
    while (true) {
        for (auto& task : tasks) {
            uint32_t current_time = bindings::GetTick();

            if (current_time - task.last_run_ms >= task.period_ms) {
                task.task();
                task.last_run_ms = current_time;
            }
        }
    }
}

}  // namespace scheduler