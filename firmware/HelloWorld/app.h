/// @author Blake Freer
/// @date 2023-12-25

#include "shared/periph/gpio.h"

template <shared::periph::DigitalInput DigitalInput>
class Button {
private:
    DigitalInput& di_;

public:
    Button(DigitalInput& di) : di_(di){};

    bool Read() {
        return di_.Read();
    }
};

template <shared::periph::DigitalOutput DigitalOutput>
class Indicator {
private:
public:
    DigitalOutput& dig_output_;
    Indicator(DigitalOutput& dig_output) : dig_output_(dig_output) {}

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