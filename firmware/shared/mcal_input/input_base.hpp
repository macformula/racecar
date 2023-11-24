/// @author Blake Freer
/// @date 2023-10-17

#ifndef MCAL_INPUT_BASE_HPP
#define MCAL_INPUT_BASE_HPP

#include "util/noncopyable.hpp"

namespace mcal {
namespace input {

/// @brief Base class for all microcontroller input sources
template <typename return_type>
class Input_Base : public util::noncopyable {

public:
    virtual return_type read() noexcept = 0;
};

} // namespace input
} // namespace mcal

#endif