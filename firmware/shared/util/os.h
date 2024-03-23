/// @author Blake Freer
/// @date 2023-11-08

#pragma once

#include "shared/util/noncopyable.h"

namespace shared::util {

// Empty for now
class os {

};

// TODO: Add more verbose error cases
enum class OsStatus : int {
    kOsOk = 0,
    kOsError
};

} // namespace shared::util
