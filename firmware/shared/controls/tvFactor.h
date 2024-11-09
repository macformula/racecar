#pragma once
#include <algorithm>
#include <tuple>
#include "../util/mappers/lookup_table.h"

namespace ctrl{

bool EnableTorqueVectoring(bool torque_vectoring_switch) {
    if (torque_vectoring_switch == true) {
        return true;
    } else {
        return false;
    }
}

template <typename T>
T CreateTorqueVectoringFactor(T steering_angle, bool torque_vectoring_switch) {
    if (EnableTorqueVectoring(torque_vectoring_switch) == true) {
        T absolute_steering_angle = std::abs(steering_angle);

        static const float steering_angle_lookup_table_data[][2] = {
            {0.0, 1.0f},    {5.0, 0.934f},  {10.0, 0.87f},
            {15.0, 0.808f}, {20.0, 0.747f}, {25.0, 0.683f},
        };

        static constexpr int steering_angle_lookup_table_length =
            (sizeof(steering_angle_lookup_table_data)) /
            (sizeof(steering_angle_lookup_table_data[0]));

        static shared::util::LookupTable<steering_angle_lookup_table_length>
            table_lookup{steering_angle_lookup_table_data};

        return table_lookup.Evaluate(absolute_steering_angle);
    } else {
        return 0;
    }
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
}
