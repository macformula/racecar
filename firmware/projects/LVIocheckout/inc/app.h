#include "shared/periph/gpio.h"

class Subsystem {
public:
    /**
     * @brief Construct a new Subsystem object
     *
     * @param enable_output Digital Output which enables the subsystem.
     * enabled).
     */
    Subsystem(shared::periph::DigitalOutput& enable_output)
        : enable_output_(enable_output) {}

    inline virtual void Enable() const {
        enable_output_.SetHigh();
    }

    inline virtual void Disable() const {
        enable_output_.SetLow();
    }

    shared::periph::DigitalOutput& enable_output_;

private:
};