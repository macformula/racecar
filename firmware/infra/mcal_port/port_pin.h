/// @author Blake Freer
/// @date 2023-10-17

#ifndef INFRA_MCAL_PORT_PIN_H_
#define INFRA_MCAL_PORT_PIN_H_

namespace mcal {
namespace port {

/// @brief Interact with a single bit in a port register
/// @note (2023-10-17) implementation to be decided later
class PortPin {
public:
    bool ReadBit(void) noexcept;

    void SetLow(void) noexcept;
    void SetHigh(void) noexcept;

    void SetDirectionOutput(void) noexcept;
    void SetDirectionInput(void) noexcept;
};

}  // namespace port
}  // namespace mcal

#endif
