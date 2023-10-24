/// @author Blake Freer
/// @date 2023-10-17

#ifndef INFRA_MCAL_DIGITAL_INPUT_BASE_H_
#define INFRA_MCAL_DIGITAL_INPUT_BASE_H_

#include "mcal_input/input_base.h"

namespace mcal {
namespace input {

/// @brief Digital input base class is just a boolean InputBase
/// @note Can be changed to a class later by inheriting from InputBase<bool>
using DigitalInputBase = InputBase<bool>;

}  // namespace input
}  // namespace mcal

#endif