/// @author Michael Nasr & Qasim Saadat
/// @date 2025-01-21

#pragma once

#include <stddef.h>

namespace shared::util {

/// @brief Recursive structure to represent a lookup table entry.
template <size_t Depth>
struct LUT_Entry {
    float key;
    float value;
    LUT_Entry<Depth - 1> next_entry;
};

/// @brief Specialization for the base case (last entry).
template <>
struct LUT_Entry<0> {
    float key;
    float value;
};

/// @brief Function to check if the table is sorted.
template <size_t Depth>
constexpr bool is_sorted(const LUT_Entry<Depth>& entry) {
    if constexpr (Depth == 0) {
        return true;
    } else {
        // Check the current key against the next and recursively check the
        // rest.
        return (entry.key < entry.next_entry.key) &&
               is_sorted(entry.next_entry);
    }
}

/// @brief Function to construct a recursive LUT structure from a 2D array.
template <size_t Depth>
constexpr LUT_Entry<Depth> construct_lut(const float (&table)[Depth + 1][2]) {
    if constexpr (Depth == 0) {
        return {table[0][0], table[0][1]};
    } else {
        // Create the current entry and recursively construct the next.
        return {table[0][0], table[0][1], construct_lut<Depth - 1>(table + 1)};
    }
}

/// @brief Function to construct a 2D array from a recursive LUT structure.
template <size_t Depth>
constexpr void lut_to_array(const LUT_Entry<Depth>& structure,
                            float (&table)[Depth + 1][2], size_t index = 0) {
    table[index][0] = structure.key;
    table[index][1] = structure.value;

    if constexpr (Depth > 0) {
        lut_to_array(structure.next_entry, table, index + 1);
    }
}

}  // namespace shared::util
