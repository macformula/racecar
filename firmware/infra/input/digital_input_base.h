/// @author Blake Freer
/// @date 2023-10-17

#ifndef INFRA_INPUT_DIGITAL_INPUT_BASE_H_
#define INFRA_INPUT_DIGITAL_INPUT_BASE_H_

#include "input/input_base.h"

namespace infra {
namespace input {

/// @brief Digital input base class is just a boolean InputBase
/// @note Can be changed to a class later by inheriting from InputBase<bool>
using DigitalInputBase = InputBase<bool>;

}  // namespace input
}  // namespace mcal

#endif