/// @author Blake Freer
/// @date 2023-11-09

#pragma once

namespace macfe::periph {

class PWMOutput {
public:
    virtual void Start() = 0;
    virtual void Stop() = 0;
    virtual void SetDutyCycle(float duty_cycle) = 0;
    virtual float GetDutyCycle() = 0;
    virtual void SetFrequency(float frequency) = 0;
    virtual float GetFrequency() = 0;
};

}  // namespace macfe::periph
