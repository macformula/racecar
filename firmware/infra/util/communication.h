/// @author Blake Freer
/// @date 2023-10-14

#ifndef INFRA_UTIL_COMMUNICATION_H_
#define INFRA_UTIL_COMMUNICATION_H_

#include "infra/util/noncopyable.h"

namespace util {

/// @brief Base class for all communication protocols (SPI, CAN, ...)
class communication_base : private util::Noncopyable {};

}  // namespace util

#endif