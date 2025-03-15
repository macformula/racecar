/// @author Blake Freer
/// @date 2024-02-08

#pragma once

#include <limits>
#include <type_traits>

namespace shared::util {

template <typename T>
concept Numeric = requires { std::is_arithmetic_v<T>; };

/// @brief Find the minimum value in an array.
/// @tparam T Datatype of the array. Must be a numeric (eg. `float` or
/// `uint8_t`).
/// @tparam array_length Length of `array`. Must be determined at compile time.
/// @param array Input array.
/// @param idx_min Implicit return pointer for the index of the minimum element.
/// Pass `nullptr` if this value is not needed. If there are multiple instances
/// of the minimum value, the least index is returned.
/// @return Minimum value of the array.
/// @note Returns a large value if `array_length = 0`.
template <Numeric T, int array_length, Numeric I>
    requires(array_length >= 0, array_length < std::numeric_limits<I>::max())
T GetMinimum(T* array, I* idx_min) {
    // start minimum at greatest value
    T cur_min = std::numeric_limits<T>().max();
    I temp_idx_min = -1;

    for (I i = 0; i < array_length; i++) {
        if (array[i] < cur_min) {
            cur_min = array[i];
            temp_idx_min = i;
        }
    }

    if (idx_min) {
        *idx_min = temp_idx_min;
    }

    return cur_min;
}

/// @brief Find the maximum value in an array.
/// @tparam T Datatype of the array. Must be a numeric (eg. `float` or
/// `uint8_t`).
/// @tparam array_length Length of `array`. Must be determined at compile time.
/// @param array Input array.
/// @param idx_min Implicit return pointer for the index of the maximum element.
/// Pass `nullptr` if this value is not needed. If there are multiple instances
/// of the maximum value, the least index is used.
/// @return Maximum value of the array.
/// @note Returns a low value if `array_length = 0`.
template <Numeric T, int array_length, Numeric I>
    requires(array_length >= 0, array_length < std::numeric_limits<I>::max())
T GetMaximum(T* array, I* idx_max) {
    // start maximum at lowest value
    T cur_max = std::numeric_limits<T>().min();
    I temp_idx_max = -1;

    for (I i = 0; i < array_length; i++) {
        if (array[i] > cur_max) {
            cur_max = array[i];
            temp_idx_max = i;
        }
    }

    if (idx_max) {
        *idx_max = temp_idx_max;
    }

    return cur_max;
}

/// @brief Calculate the average value of an array.
/// @tparam T Datatype of the array. Must be a numeric (eg. `float` or
/// `uint8_t`).
/// @tparam array_length Length of `array`. Must be determined at compile time.
/// @param array Input array.
/// @return Average value of the array, as a `float`.
template <Numeric T, int array_length>
    requires(array_length > 0)
float GetAverage(T* array) {
    float sum = 0;
    for (int i = 0; i < array_length; i++) {
        sum += array[i];
    }
    return sum / array_length;
}

}  // namespace shared::util