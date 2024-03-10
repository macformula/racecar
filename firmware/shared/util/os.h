/// @author Blake Freer
/// @date 2023-11-08

// #pragma once
#ifndef SHARED_UTIL_OS_H_
#define SHARED_UTIL_OS_H_

#include "shared/util/noncopyable.h"

namespace shared::util {

// Empty for now
class os {

};

// TODO: Add more verbose error cases
enum class osStatus : int {
    osOk = 0,
    osError
};

} // namespace shared::util

#endif