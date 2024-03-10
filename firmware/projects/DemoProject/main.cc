/// @author Blake Freer, Matteo Tullo
/// @date 2023-12-25

#include <cstdint>
#include <cstring>

#include "app.h"

namespace bindings {
extern shared::periph::DigitalInput& button_di;
extern shared::periph::DigitalOutput& indicator_do;
extern shared::periph::DigitalOutput& indicator2_do;
extern void Initialize();
}   // namespace bindings

namespace os {
extern shared::osDataType::Semaphore& sem_test;
extern shared::osDataType::Fifo& message_queue_test;
extern shared::osDataType::Timer& timer_test;
extern void InitializeKernel();
extern void StartKernel();
}   // namespace os

extern "C" {
void StartTestTask(void *argument);
void StartLedTask(void *argument);
void startMessageSendTask(void *argument);
void messageTimerCallback(void *argument);
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

    for (;;)
    {

    }

    return 0;
}

// Poll the button and, when pressed, signal the semaphore
// and sleep for half a second
void StartTestTask(void *argument) {
    uint32_t delayTime = shared::os::GetTickCount();
    for (;;) {
        if (button.Read() == true) {
            os::sem_test.Release();
            delayTime += 500;
            shared::os::TickUntil(delayTime);
        }
        shared::os::Tick(1);
    }
}

// Blink the red LED once a signal is received from the semaphore
void StartLedTask(void *argument) {
    bool val = false;
    indicator2.Set(val);
    for (;;) {
        os::sem_test.Acquire();
        indicator2.Set(val);
        val = !val;
    }
}

// Start the oneshot timer and check for the message to be received
// After 3 seconds, the blue LED turns on to signal the message is received
// The task then terminates
void startMessageSendTask(void *argument) {
    char buf[16] = { 0 };
    uint8_t buf_index = 0;
    indicator.Set(false);
    os::timer_test.Start(3000);
    for (;;) {
        if (os::message_queue_test.Get(static_cast<void*>(&buf[buf_index]), nullptr) == shared::util::osStatus::osOk) {
            buf_index++;
        }

        if (buf_index >= sizeof(msg)) {
            if (strcmp(const_cast<const char*>(buf), msg) == 0) {
                // Received Hello World!
                indicator.Set(true);
                // TODO: Create task terminate
                shared::os::Tick(60000);
            }
        }
        shared::os::Tick(1);
    }
}

// Oneshot timer callback
// Write message to the FIFO
void messageTimerCallback(void *argument) {
    if (os::message_queue_test.GetSpaceAvailable() >= sizeof(msg)) {
        for (unsigned i=0; i<sizeof(msg); i++) {
            os::message_queue_test.Put(static_cast<const void*>(&msg[i]), 0);
        }
    }
}
