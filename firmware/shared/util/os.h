/// @author Blake Freer
/// @date 2023-11-08

// #pragma once
#ifndef SHARED_UTIL_OS_H_
#define SHARED_UTIL_OS_H_

#include "shared/util/noncopyable.h"

namespace shared::util {


// TODO: Update comment
/** 
 * This class provides a generic parent that all os related
 * objects will inherit from.
 * It is currently empty but it still included for future use.
 * 
 * Inheritance is required by the concept constraint
 * `std::is_base_of_v<util::Peripheral, T>;`
 * See `shared/periph/adc.c` for an example.
 * 
 * Inheritance is performed in concept implementation
 * `class ADCInput : public shared::util::Peripheral`
 * See `mcal/.../periph/adc.h` for an example
*/
class os {

};

enum class osStatus : int {
    osOk = 0,
    osError
};

} // namespace shared::util

#endif