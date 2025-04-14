/// @author Blake Freer
/// @date 2025-04

#pragma once

#include <span>
#include <type_traits>

#include "mapper.hpp"

namespace shared::util {

/// @brief Linearly interpolates the input value according to a table to form a
/// piecewise lineary approximation of a function.
/// @note If the input `key` is less than the lowest key in the table, the the
/// value of the lowest key is returned, and similarly for the largest key.
template <typename T = float>
    requires std::is_floating_point_v<T>
class LookupTable : public Mapper<T> {
public:
    struct Entry {
        T key;
        T value;
    };
    // Use a span so that LookupTable can be instantiated with different sized
    // arrays. Directly using an array would force LookupTable<N> to have fixed
    // size.
    using LUTData = std::span<const Entry>;

    /// @warning The table's first columns (keys) must be sorted in increasing
    /// order.
    LookupTable(const LUTData table) : table_(table) {}

    inline T Evaluate(T key) const override {
        return LookupTable::Evaluate(table_, key);
    }

    static T Evaluate(const LUTData table, T key) {
        int least_greater_idx = 0;

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

private:
    const LUTData table_;
};

}  // namespace shared::util
