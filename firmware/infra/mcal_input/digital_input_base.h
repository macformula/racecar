/// @author Blake Freer
/// @date 2023-10-17

#ifndef MCAL_DIGITAL_INPUT_BASE_HPP
#define MCAL_DIGITAL_INPUT_BASE_HPP

#include "mcal_input/input_base.h"

namespace mcal {
namespace input {

/// @brief Digital input base class is just a boolean Input_Base
/// @note Can be changed to a class later by inheriting from Input_Base<bool>
using DigitalInput_Base = Input_Base<bool>;

}  // namespace input
}  // namespace mcal

#endif