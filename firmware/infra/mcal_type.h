/// @author Blake Freer
/// @date 2023-10-28
/// @brief Define concepts to ensure mcal layer classes have the necessary properties

#include <concepts>

#ifndef INFRA_MCAL_TYPE_H_
#define INFRA_MCAL_TYPE_H_

namespace mcal {

namespace gpio {
/// @brief Specifies the requirements for a class to act as a "GpioPin"
template<typename pin>
concept GpioPin = requires {
	{pin::Init()} -> std::same_as<void>;
	{pin::Read()} -> std::same_as<bool>;
	{pin::Set()} -> std::same_as<void>;
	{pin::Clear()} -> std::same_as<void>;
};

} // namespace gpio

} // namespace mcal

#endif