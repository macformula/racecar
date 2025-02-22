/// @author Michael Nasr & Qasim Saadat
/// @date 2025-01-21

#pragma once

#include <stddef.h>

namespace shared::util {

template <size_t Depth, typename T>
struct LUT_Entry {
    T key;
    T value;
    LUT_Entry<Depth - 1, T> next_entry;
};

template <typename T>
struct LUT_Entry<0, T> {
    T key;
    T value;
};

template <size_t Depth, typename T>
constexpr bool is_sorted(const LUT_Entry<Depth, T>& entry) {
    if constexpr (Depth == 0) {
        return true;
    } else {
        return (entry.key < entry.next_entry.key) &&
               is_sorted(entry.next_entry);
    }
}

template <size_t Depth, typename T>
constexpr LUT_Entry<Depth, T> construct_lut(const T (&table)[Depth + 1][2]) {
    if constexpr (Depth == 0) {
        return {table[0][0], table[0][1]};
    } else {
        // Ensure correct subarray passing
        T sub_table[Depth][2];
        for (size_t i = 0; i < Depth; ++i) {
            sub_table[i][0] = table[i + 1][0];
            sub_table[i][1] = table[i + 1][1];
        }
        return {table[0][0], table[0][1],
                construct_lut<Depth - 1, T>(sub_table)};
    }
}

template <size_t Depth, typename T>
constexpr void lut_to_array(const LUT_Entry<Depth, T>& structure,
                            T (&table)[Depth + 1][2], size_t index = 0) {
    table[index][0] = structure.key;
    table[index][1] = structure.value;

    if constexpr (Depth > 0) {
        lut_to_array(structure.next_entry, table, index + 1);
    }
}

}  // namespace shared::util
