#include <string>

#include "periph/pwm.hpp"

namespace mcal::lnx {

class PWMOutput : public macfe::periph::PWMOutput {
public:
    PWMOutput(std::string name);

    void Start() override;
    void Stop() override;
    void SetDutyCycle(float duty_cycle) override;
    float GetDutyCycle() override;
    void SetFrequency(float frequency) override;
    float GetFrequency() override;

private:
    float duty_;
    float frequency_;
    std::string name_;
};

}  // namespace mcal::lnx