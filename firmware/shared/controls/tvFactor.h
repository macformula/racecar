#pragma once
#include <algorithm>
#include <tuple>

#include "shared/util/mappers/lookup_table.hpp"

namespace ctrl {

template <typename T>
T CreateTorqueVectoringFactor(T steering_angle) {
    T absolute_steering_angle = std::abs(steering_angle);

    static const float table_data[][2] = {{0.0, 1.0f},    {5.0, 0.934f},
                                          {10.0, 0.87f},  {15.0, 0.808f},
                                          {20.0, 0.747f}, {25.0, 0.683f}};

    static constexpr int table_length =
        (sizeof(table_data)) / (sizeof(table_data[0]));

    static shared::util::LookupTable<table_length> tv_lookup_table{table_data};

    return tv_lookup_table.Evaluate(absolute_steering_angle);
}

template <typename T>
struct TorqueVector {
    T left;
    T right;
};

template <typename T>
TorqueVector<T> AdjustTorqueVectoring(T steering_angle) {
    TorqueVector<T> torque_vector;

    T torque_vectoring_factor = CreateTorqueVectoringFactor(steering_angle);

    if (steering_angle > 0) {
        torque_vector.left = static_cast<T>(1);
        torque_vector.right = torque_vectoring_factor;
    } else if (steering_angle < 0) {
        torque_vector.right = static_cast<T>(1);
        torque_vector.left = torque_vectoring_factor;
    } else {
        torque_vector.left = static_cast<T>(1);
        torque_vector.right = static_cast<T>(1);
    }

    return torque_vector;
}
}  // namespace ctrl
