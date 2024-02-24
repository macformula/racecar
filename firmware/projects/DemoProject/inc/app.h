/// @author Blake Freer
/// @date 2023-12-25

#include "shared/periph/gpio.h"
#include "shared/os/semaphore.h"
#include "shared/util/os.h"

class Button {
private:
    shared::periph::DigitalInput& di_;

public:
    Button(shared::periph::DigitalInput& di) : di_(di){};

    bool Read() {
        return di_.Read();
    }
};

class Indicator {
private:
public:
    shared::periph::DigitalOutput& dig_output_;
    Indicator(shared::periph::DigitalOutput& dig_output)
        : dig_output_(dig_output) {}

    void Set(bool value) {
        dig_output_.Set(value);
    }

    void High() {
        dig_output_.SetHigh();
    }

    void Low() {
        dig_output_.SetLow();
    }
};

// class Semaphore {
// private:
// public:
//     Semaphore_& sem;
//     Semaphore(Semaphore_& sem_) : sem(sem_) {}

//     shared::util::osStatus Wait() {
//         return sem.Wait();
//     }

//     shared::util::osStatus Post() {
//         return sem.Post();
//     }

//     uint32_t GetCount() {
//         return sem.GetCount();
//     }
// };