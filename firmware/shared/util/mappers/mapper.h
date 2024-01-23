/// @author Blake Freer
/// @date 2024-01-22

#ifndef SHARED_UTIL_MAPPERS_MAPPER_H_
#define SHARED_UTIL_MAPPERS_MAPPER_H_

#include <concepts>

namespace shared::util {

template <typename T>
concept Mapper = requires(T obj, float key) {
    { obj.Evaluate(key) } -> std::same_as<float>;
};

}  // namespace shared::util

#endif