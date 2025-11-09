#pragma once

#include <cstdint>
#include <cstdlib>

namespace macfe::periph {

class SpiMaster {
public:
    virtual ~SpiMaster() = default;
    /// Transmit `length` bytes from `tx_data`
    virtual void Transmit(uint8_t* tx_data, size_t length) {}

    /// Receive `length` bytes and place them into `rx_data`
    virtual void Receive(uint8_t* rx_data, size_t length) {}

    /// Transmit `length` bytes from `tx_data` and receive `length` bytes,
    /// placing them into `rx_data`
    virtual void TransmitReceive(uint8_t* tx_data, uint8_t* rx_data,
                                 size_t length) {}
};

}  // namespace macfe::periph