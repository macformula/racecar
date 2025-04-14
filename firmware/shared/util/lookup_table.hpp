/// @author Blake Freer
/// @date 2025-04

#pragma once

#include <span>
#include <type_traits>

namespace shared::util {

template <typename T>
    requires std::is_floating_point_v<T>
struct LookupTable {
    struct Entry {
        T key;
        T value;
    };

    // Use a span so that LookupTable::Evalute can be called with different
    // sized tables.
    using Data = std::span<const Entry>;

    /// @brief Linearly interpolates the input value according to a table to
    /// form a piecewise lineary approximation of a function.
    ///
    /// @note If the input `key` is less than the lowest key in the table, the
    /// the value of the lowest key is returned, and similarly for the largest
    /// key.
    ///
    /// @warning The table's first columns (keys) must be sorted in increasing
    /// order.
    static T Evaluate(const Data table, T key) {
        size_t least_greater_idx = 0;

        // Find next greatest element in keys_, assumes keys_ is sorted
        while (table[least_greater_idx].key < key &&
               least_greater_idx < table.size()) {
            least_greater_idx += 1;
        }

        // If key is outside of range, return edge value
        if (least_greater_idx == 0) {
            return table.front().value;
        }
        if (least_greater_idx == table.size()) {
            return table.back().value;
        }

        auto prev = table[least_greater_idx - 1];
        auto next = table[least_greater_idx];

        T fraction = (key - prev.key) / (next.key - prev.key);

        return (1 - fraction) * prev.value + fraction * next.value;
    }
};

}  // namespace shared::util
