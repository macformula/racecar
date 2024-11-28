/// @author Blake Freer
/// @date 2023-10-14

#pragma once

#include "shared/util/noncopyable.hpp"

namespace shared::util {

/// @brief Base class for all communication protocols (SPI, CAN, ...)
class communication_base : private Noncopyable {};

}  // namespace shared::util
