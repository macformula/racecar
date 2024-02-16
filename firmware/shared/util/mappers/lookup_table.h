/// @author Blake Freer
/// @date 2023-12-04

/**
 * @todo (BlakeFreer) This lookup table requires the keys to be sorted in
 * increasing order. Write a compile time checker that fails if this is not the
 * case (iterate over) keys, if one is less than previous, raise error.
 */

#pragma once

#include "mapper.h"

namespace shared::util {

/**
 * @brief Linearly interpolates the input value according to a table to forma
 * piecewise lineary approximation of a function.
 * @tparam row_count_ The number of rows in the lookup table. Must be a compile
 * time constant.
 * @note If the input `key` is less than the lowest key in the table, the the
 * value of the lowest key is returned, and similarly for the largest key.
 */
template <int row_count_>
class LookupTable : public Mapper<float> {
public:
    /**
     * @warning The table's first columns (keys) must be sorted in increasing
     * order.
     */
    LookupTable(float const (*table)[2]) : table_(table) {}

    inline float Evaluate(float key) const override {
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

        float fraction =
            (key - table_[least_greater_idx - 1][0]) /
            (table_[least_greater_idx][0] - table_[least_greater_idx - 1][0]);

        float interpolated_value =
            (1 - fraction) * table_[least_greater_idx - 1][1] +
            fraction * table_[least_greater_idx][1];

        return interpolated_value;
    }

private:
    const float (*table_)[2];
};

}  // namespace shared::util
