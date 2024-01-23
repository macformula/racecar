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

template <Mapper MapF, Mapper MapG>
class CompositeMap {
public:
    CompositeMap(MapF& f, MapG& g) : f_(f), g_(g) {}

    float Evaluate(float key) {
        return f_.Evaluate(g_.Evaluate(key));
    }

private:
    MapF& f_;
    MapG& g_;
};

}  // namespace shared::util

#endif