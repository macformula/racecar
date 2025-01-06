#pragma once
#include <algorithm>
#include <tuple>

#include "shared/util/mappers/lookup_table.hpp"

namespace ctrl {

template <typename T>
T CreateTorqueVectoringFactor(T steering_angle) {
    T absolute_steering_angle = std::abs(steering_angle);

    static const float table_data[][2] = {
        {0.0, 1.0f},    {5.0, 0.934f},  {10.0, 0.87f},
        {15.0, 0.808f}, {20.0, 0.747f}, {25.0, 0.683f},
    };

    static constexpr int table_length =
        (sizeof(table_data)) / (sizeof(table_data[0]));

    static shared::util::LookupTable<table_length> tv_lookup_table{table_data};

    return tv_lookup_table.Evaluate(absolute_steering_angle);
}

template <typename T>
std::tuple<T, T> AdjustTorqueVectoring(T steering_angle,
                                       T torque_vectoring_factor) {
    T left_torque_vector;
    T right_torque_vector;

    if (steering_angle > 0) {
        left_torque_vector = static_cast<T>(1);
        right_torque_vector = torque_vectoring_factor;
    } else if (steering_angle < 0) {
        right_torque_vector = static_cast<T>(1);
        left_torque_vector = torque_vectoring_factor;
    } else {
        left_torque_vector = static_cast<T>(1);
        right_torque_vector = static_cast<T>(1);
    }

    return std::tuple(left_torque_vector, right_torque_vector);
}
}  // namespace ctrl
