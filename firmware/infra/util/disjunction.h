/// @author Blake Freer
/// @date 2023-10-26
/// @note See https://github.com/tomikaa87/stm32-hal-cpp

#ifndef INFRA_UTIL_DISJUNCTION_H_
#define INFRA_UTIL_DISJUNCTION_H_

#include <cstdint>

namespace util {

template <uint32_t... values>
struct Disjunction;

template <uint32_t first, uint32_t... others>
struct Disjunction<first, others...> {
    static const uint32_t value = first | Disjunction<others...>::value;
};

template <>
struct Disjunction<> {
    static const uint32_t value = 0;
};


template<uint8_t... values>
struct FlagDisjunction;

template <uint8_t first, uint8_t... others>
struct FlagDisjunction<first, others...> {
	static_assert(first < 32, "maximum 32 bits can be used");
	static const uint32_t value = (1 << first) | FlagDisjunction<others...>::value;
};

template <>
struct FlagDisjunction<> {
	static const uint32_t value = 0;
};

}  // namespace util

#endif