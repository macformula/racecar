#pragma once

#include "shared/os/fifo.hpp"
#include "shared/os/semaphore.hpp"
#include "shared/os/timer.hpp"
#include "shared/periph/gpio.hpp"

namespace bindings {

extern shared::periph::DigitalInput& button_di;
extern shared::periph::DigitalOutput& indicator_do;
extern shared::periph::DigitalOutput& indicator2_do;
extern void Initialize();

}  // namespace bindings

namespace os {

extern shared::os::Semaphore& sem_test;
extern shared::os::Fifo& message_queue_test;
extern shared::os::Timer& timer_test;
extern void Tick(uint32_t ticks);
extern void TickUntil(uint32_t ticks);
extern uint32_t GetTickCount();
extern void InitializeKernel();
extern void StartKernel();

}  // namespace os