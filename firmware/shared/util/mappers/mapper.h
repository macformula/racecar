/// @author Blake Freer
/// @date 2024-01-22

#ifndef SHARED_UTIL_MAPPERS_MAPPER_H_
#define SHARED_UTIL_MAPPERS_MAPPER_H_

namespace shared::util {

class Mapper {
public:
    virtual float Evaluate(float key) = 0;
};

class CompositeMap : public Mapper {
public:
    CompositeMap(Mapper& f, Mapper& g) : f_(f), g_(g) {}

    float Evaluate(float key) override {
        return f_.Evaluate(g_.Evaluate(key));
    }

private:
    Mapper& f_;
    Mapper& g_;
};

}  // namespace shared::util

#endif