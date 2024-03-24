/// @author Blake Freer
/// @date 2023-11-08

#pragma once

namespace shared::os {

// TODO: Add more verbose error cases
enum class OsStatus : int {
    kOk = 0,
    kError = 1
};

}  // namespace shared::os
