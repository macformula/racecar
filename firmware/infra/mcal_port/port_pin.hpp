/// @author Blake Freer
/// @date 2023-10-17

#ifndef MCAL_PORT_HPP
#define MCAL_PORT_HPP

namespace mcal {
namespace port {

/// @brief Interact with a single bit in a port register
/// @note (2023-10-17) implementation to be decided later
class port_pin {
public:
    bool read_bit(void) noexcept;

    void set_low(void) noexcept;
    void set_high(void) noexcept;

    void set_direction_output(void) noexcept;
    void set_direction_input(void) noexcept;
};

}  // namespace port
}  // namespace mcal

#endif
