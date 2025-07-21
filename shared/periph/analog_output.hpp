/// @author Tamara Xu
/// @date 2024-10-03

#pragma once

namespace shared::periph {

class AnalogOutput {
public:
    virtual void SetVoltage(float voltage) = 0;
};

}  // namespace shared::periph