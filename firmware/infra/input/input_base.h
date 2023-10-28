/// @author Blake Freer
/// @date 2023-10-17

#ifndef INFRA_INPUT_INPUT_BASE_H_
#define INFRA_INPUT_INPUT_BASE_H_

#include "util/noncopyable.h"

namespace infra {
namespace input {

/// @brief Base class for all microcontroller input sources
template <typename ReturnType>
class InputBase : public util::Noncopyable {
public:
    virtual ReturnType read() noexcept = 0;
};

}  // namespace input
}  // namespace mcal

#endif