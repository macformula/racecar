// /// @author Matteo Tullo
// /// @date 2024-02-02

// #ifndef SHARED_OS_TASK_H_
// #define SHARED_OS_TASK_H_

// #include <concepts>
// #include "shared/util/os.h"

// namespace shared::periph {

// template <typename T>
// concept Task = requires(T obj) {
// 	{ obj.Read() } -> std::same_as<bool>;

// 	std::is_base_of_v<util::os, T>;
// };

// template <typename T>
// concept DigitalOutput = requires(T obj, bool value) {
// 	{ obj.SetHigh() } -> std::same_as<void>;
// 	{ obj.SetLow() } -> std::same_as<void>;
// 	{ obj.Set(value) } -> std::same_as<void>;

// 	std::is_base_of_v<util::Peripheral, T>;
// };

// } // namespace shared::periph

// #endif