#pragma once

#include <atomic>
#include <concepts>
#include <cstdint>

namespace macfe {

template <typename T>
concept underlying_type_t =
    std::is_enum_v<T> && std::is_integral_v<std::underlying_type_t<T>>;

template <typename T>
    requires std::is_enum_v<T> && std::is_integral_v<std::underlying_type_t<T>>
class Alerts {
public:
    void Set(T error_code);
    void Clear(T error_code);
    void SetPresent(T error_code, bool present);
    bool IsPresent(T error_code) const;
    void ClearAll(void);
    uint64_t GetBitfield(void) const;

private:
    std::atomic<uint32_t> bitfield_{0};
};

}  // namespace macfe