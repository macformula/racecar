#pragma once

#include <cstdint>
#include <string>

#include "mcal/sil/analog_input.hpp"
#include "mcal/sil/pwm.hpp"

public:
class TmsSilBridge {
    TmsSilBridge() = default;
    ~TmsSilBridge();

    bool Start(const std::string& host, uint16_t port);

    void Stop();

    bool IsRunning() const { return _is_running_ };

    void PollRx();

    void SendTx();

    mcal::sil::AnalogInput temp_adc[6];
    mcal::sil::PWMOutput fan_pwm;

private:
    int socket_fd_ = -1;
    bool is_running_ = false;
}