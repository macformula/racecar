/// @author Blake Freer
/// @date 2023-10-14

#ifndef SHARED_UTIL_COMMUNICATION_H_
#define SHARED_UTIL_COMMUNICATION_H_

#include "shared/util/noncopyable.h"

namespace shared::util {

/// @brief Base class for all communication protocols (SPI, CAN, ...)
class communication_base : private Noncopyable {};

}  // namespace shared::util

#endif