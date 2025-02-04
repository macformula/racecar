/// @author Samuel Parent
/// @date 2025-02-04

#pragma once

namespace shared::util {

/**
 * This class provides a generic parent that all devices will inherit from.
 * It is currently empty but it still included for future use.
 *
 * Inheritance is required by the concept constraint
 * `std::is_base_of_v<util::Device, T>;`
 * See `shared/device/dac.hpp` for an example.
 *
 * Inheritance is performed in concept implementation
 * `class DigitalAnalogConverter : public shared::util::Device`
 * See `shared/device/dac.hpp` for an example
 */
class Device {};

}  // namespace shared::util