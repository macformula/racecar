#include "alerts.hpp"

namespace macfe {

// Convert an alert enum value into a single bit mask
template <typename T>
uint64_t MakeMask(T error_code) {
    return 1ULL << static_cast<uint32_t>(error_code);
}

template <typename T>
    requires std::is_enum_v<T> && std::is_integral_v<std::underlying_type_t<T>>
void Alerts<T>::Set(T error_code) {
    // Turn this alert on
    uint64_t mask = MakeMask(error_code);
    bitfield_.fetch_or(mask, std::memory_order_relaxed);
}

template <typename T>
    requires std::is_enum_v<T> && std::is_integral_v<std::underlying_type_t<T>>
void Alerts<T>::Clear(T error_code) {
    // Turn this alert off
    uint64_t mask = MakeMask(error_code);
    bitfield_.fetch_and(~mask, std::memory_order_relaxed);
}

template <typename T>
    requires std::is_enum_v<T> && std::is_integral_v<std::underlying_type_t<T>>
void Alerts<T>::SetPresent(T error_code, bool present) {
    // Set or clear based on the flag
    present ? Set(error_code) : Clear(error_code);
}

template <typename T>
    requires std::is_enum_v<T> && std::is_integral_v<std::underlying_type_t<T>>
bool Alerts<T>::IsPresent(T error_code) const {
    // Check if this alert is currently active
    uint64_t mask = MakeMask(error_code);
    return (bitfield_.load(std::memory_order_relaxed) & mask) != 0;
}

template <typename T>
    requires std::is_enum_v<T> && std::is_integral_v<std::underlying_type_t<T>>
void Alerts<T>::ClearAll() {
    // Clear all active alerts
    bitfield_.store(0, std::memory_order_relaxed);
}

template <typename T>
    requires std::is_enum_v<T> && std::is_integral_v<std::underlying_type_t<T>>
uint64_t Alerts<T>::GetBitfield() const {
    // Return the raw alert bitfield
    return static_cast<uint64_t>(bitfield_.load(std::memory_order_relaxed));
}

}  // namespace macfe
