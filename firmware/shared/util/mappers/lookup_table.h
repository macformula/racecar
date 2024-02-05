/// @author Blake Freer
/// @date 2023-12-04

/**
 * @note We should create a concept / abstract class representing a "mapper"
 * function that would represent the functionality of a LookupTable, linear
 * mapping function, etc such that any range conversion can use a unified type
 *
 * @todo (BlakeFreer) This lookup table requires the keys to be sorted in
 * increasing order. Write a compile time checker that fails if this is not the
 * case (iterate over) keys, if one is less than previous, raise error.
 */

#ifndef SHARED_UTIL_MAPPERS_LOOKUP_TABLE_H_
#define SHARED_UTIL_MAPPERS_LOOKUP_TABLE_H_

#include "mapper.h"

namespace shared::util {

template <int row_count_>
class LookupTable : public Mapper {
public:
    LookupTable(float const (*table)[2]) : table_(table) {}

    float Evaluate(float key) const override {
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
    float const (*table_)[2];
};

}  // namespace shared::util

#endif