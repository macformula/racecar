/// @author Blake Freer
/// @date 2023-11-01

#ifndef INFRA_OUTPUT_OUTPUT_BASE_H_
#define INFRA_OUTPUT_OUTPUT_BASE_H_

#include "util/noncopyable.h"

namespace infra {
namespace output {

/// @brief Base class for all microcontroller outputs
template <typename ValueType>
class OutputBase : public util::Noncopyable {
public:
	virtual void Set(ValueType value) noexcept = 0;
};

} // namespace output
} // namespace infra

#endif