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

    /// @warning The table's first columns (keys) must be sorted in increasing
    /// order.
    explicit LookupTable(std::span<Entry> table) : table_(table) {}

    inline T Evaluate(T key) const override {
        int least_greater_idx = 0;

        // Find next greatest element in keys_, assumes keys_ is sorted
        while (table_[least_greater_idx].key < key &&
               least_greater_idx < table_.size()) {
            least_greater_idx += 1;
        }

        // If key is outside of range, return edge value
        if (least_greater_idx == 0) {
            return table_.front().value;
        }
        if (least_greater_idx == table_.size()) {
            return table_.back().value;
        }

        auto prev = table_[least_greater_idx - 1];
        auto next = table_[least_greater_idx];

        T fraction = (key - prev.key) / (next.key - prev.key);

        return (1 - fraction) * prev.value + fraction * next.value;
    }

private:
    const std::span<Entry> table_;
};

}  // namespace shared::util
