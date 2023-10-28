/// @author Blake Freer
/// @date 2023-10-28
/// @brief Define concepts to ensure mcal layer classes have the necessary properties

#include <concepts>

#ifndef INFRA_MCAL_TYPE_H_
#define INFRA_MCAL_TYPE_H_

namespace mcal {
namespace type {

template<typename pin>
/// @brief Specifies the requirements for a class to act as a "GpioPin"
concept GpioPin = requires {
	{pin::init()} -> std::same_as<void>;
	{pin::read()} -> std::same_as<bool>;
	{pin::set()} -> std::same_as<void>;
	{pin::reset()} -> std::same_as<void>;
};

} // namespace type
} // namespace mcal

#endif