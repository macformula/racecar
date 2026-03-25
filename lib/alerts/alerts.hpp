#pragma once

#include <concepts>
#include <cstdint>
#include <cstdlib>

namespace macfe {

template <typename T>
    requires std::is_enum_v<T> && std::is_integral_v<std::underlying_type_t<T>>
class Alerts {
    void Set(T error_code);
    void Clear(T error_code);
    void SetPresent(T error_code, bool present);

    bool IsPresent(T error_code);

    void ClearAll(void);

private:
    uint64_t bitfield;
};
}  // namespace macfe
