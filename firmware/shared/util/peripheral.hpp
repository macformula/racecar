/// @author Blake Freer
/// @date 2023-11-08

#pragma once

#include "shared/util/noncopyable.hpp"

namespace shared::util {

/**
 * This class provides a generic parent that all peripherals will inherit from.
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
class Peripheral {};

}  // namespace shared::util
