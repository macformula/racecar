/// @author Blake Freer, Matteo Tullo
/// @date 2023-12-25

#include <cstdint>
#include <cstring>

#include "inc/app.h"
#include "shared/os/fifo.h"
#include "shared/os/mutex.h"
#include "shared/os/os.h"
#include "shared/os/timer.h"

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

extern "C" {
void TestTask(void* argument);
void LedTask(void* argument);
void MessageSendTask(void* argument);
void MessageTimerCallback(void* argument);
}

Button button{bindings::button_di};
Indicator indicator{bindings::indicator_do};
Indicator indicator2{bindings::indicator2_do};

bool btn_value;

const char msg[] = "Hello World!";

int main(void) {
    bindings::Initialize();
    os::InitializeKernel();

    os::StartKernel();

    while (true) continue;  // logic is handled by OS tasks

    return 0;
}

// Poll the button and, when pressed, signal the semaphore
// and sleep for half a second
void TestTask(void* argument) {
    uint32_t delay_time = os::GetTickCount();
    while (true) {
        if (button.Read() == true) {
            os::sem_test.Release();
            delay_time += 500;
            os::TickUntil(delay_time);
        }
        os::Tick(1);
    }
}

// Blink the red LED once a signal is received from the semaphore
void LedTask(void* argument) {
    bool val = false;
    indicator2.Set(val);
    while (true) {
        os::sem_test.Acquire();
        indicator2.Set(val);
        val = !val;
    }
}

// Start the oneshot timer and check for the message to be received
// After 3 seconds, the blue LED turns on to signal the message is received
// The task then terminates
void MessageSendTask(void* argument) {
    char buf[16] = {0};
    uint8_t buf_index = 0;
    indicator.Set(false);
    os::timer_test.Start(3000);
    while (true) {
        shared::os::OsStatus get_status = os::message_queue_test.Get(
            static_cast<void*>(&buf[buf_index]), nullptr);
        if (get_status == shared::os::OsStatus::kOk) {
            buf_index++;
        }

        if (buf_index >= sizeof(msg)) {
            if (strcmp(const_cast<const char*>(buf), msg) == 0) {
                // Received Hello World!
                indicator.Set(true);
                // TODO: Create task terminate
                os::Tick(60000);
            }
        }
        os::Tick(1);
    }
}

// Oneshot timer callback
// Write message to the FIFO
void MessageTimerCallback(void* argument) {
    if (os::message_queue_test.GetSpaceAvailable() >= sizeof(msg)) {
        for (unsigned i = 0; i < sizeof(msg); i++) {
            os::message_queue_test.Put(static_cast<const void*>(&msg[i]), 0);
        }
    }
}
