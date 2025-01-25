/// @author Blake Freer
/// @date 2023-12-04

#pragma once

#include <type_traits>

#include "mapper.hpp"

namespace shared::util {

/// @brief Linearly interpolates the input value according to a table to form a
/// piecewise lineary approximation of a function.
/// @tparam row_count_ The number of rows in the lookup table. Must be a compile
/// time constant.
/// @note If the input `key` is less than the lowest key in the table, the the
/// value of the lowest key is returned, and similarly for the largest key.
template <int row_count_, typename T = float>
    requires std::is_floating_point_v<T>
class LookupTable : public Mapper<T> {
public:
    /// @brief Constructs a LookupTable with a 2D array of key-value pairs.    
    /// @warning The table's first columns (keys) must be sorted in increasing
    /// order.
    constexpr LookupTable(T const (*table)[2]) : table_(table) {
        // Construct the recursive LUT structure from the table.
        lut_ = construct_lut<row_count_ - 1>(table);

        // Check if the table is sorted at compile time.
        if (!is_sorted(lut_)) {
            throw "Lookup table keys must be sorted.";
        }
        
        // Convert the LUT structure back into an array.
        float array_[row_count_ - 1][2];
        lut_to_array(lut_, array_);

    }

    inline T Evaluate(T key) const override {
        int least_greater_idx = 0;

        // Find next greatest element in keys_, assumes keys_ is sorted
        while (table_[least_greater_idx][0] < key &&
               least_greater_idx < row_count_) {
            least_greater_idx += 1;
        }

        // If key is outside of range, return edge value
        if (least_greater_idx == 0) {
            return table_[0][1];
        }
        if (least_greater_idx == row_count_) {
            return table_[row_count_ - 1][1];
        }

        auto [prev_key, prev_val] = table_[least_greater_idx - 1];
        auto [next_key, next_val] = table_[least_greater_idx];

        T fraction = (key - prev_key) / (next_key - prev_key);

        return (1 - fraction) * prev_val + fraction * next_val;
    }

private:
    const T (*table_)[2];
};

}  // namespace shared::util
