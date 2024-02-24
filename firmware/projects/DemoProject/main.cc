/// @author Blake Freer
/// @date 2023-12-25

#include <cstdint>

#include "app.h"
#include "os.h"

namespace bindings {
extern shared::periph::DigitalInput& button_di;
extern shared::periph::DigitalOutput& indicator_do;
extern void Initialize();
}  // namespace bindings

namespace os {
extern mcal::os::Semaphore sem_test;
extern mcal::os::Mutex mutex_test;
}


extern "C" {
void StartTestTask(void *argument);
void StartLedTask(void *argument);
}

Button button{bindings::button_di};
Indicator indicator{bindings::indicator_do};
Indicator indicator2{bindings::indicator2_do};

bool btn_value;

int main(void) {
    os::InitializeKernel();

    bindings::Initialize();

    os::StartKernel();

    for (;;)
    {

    }

    return 0;
}

// TODO: 

void StartTestTask(void *argument) {
    bool val = true;
    uint32_t delayTime = shared::os::GetTickCount();
    for (;;) {
        if (button.Read() == true) {
            os::sem_test.Release();
            delayTime += 100;
            shared::os::TickUntil(delayTime);
        }
    }
}

void StartLedTask(void *argument) {
    bool val = false;
    indicator.Set(false);
    indicator2.Set(false);
    for (;;) {
        os::sem_test.Acquire();
        indicator2.Set(val);
        val = !val;
    }
}
