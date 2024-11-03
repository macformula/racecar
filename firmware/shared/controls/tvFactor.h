#pragma once
#include <cmath>
#include <vector>
#include <algorithm>
#include <tuple>
#include "../util/mappers/lookup_table.h"

namespace ctrl{

template <typename T>
T tvEnable(T DI_p_steeringAngle, T DI_b_tvEnable){

    if (DI_b_tvEnable > 0) return DI_p_steeringAngle;
    else return 0;
}

template <typename T>
T open_loop_tv_lookUp(T DI_p_steeringAngle){

    T Angle = std::abs(DI_p_steeringAngle);

    // Lookup Table Data
    // breakpoints = {0, 5, 10, 15, 20, 25}; 
    // tableData = {1, 0.934, 0.87, 0.808, 0.747, 0.683};

    const float steeringAngle_lut_data[][2] = {
    {0.0, 1.0f},
	{5.0, 0.934f},
	{10.0, 0.87f},
	{15.0, 0.808f},
	{20.0, 0.747f},
	{25.0, 0.683f},
    };

    constexpr int steeringAngle_lut_length =
    (sizeof(steeringAngle_lut_data)) / (sizeof(steeringAngle_lut_data[0]));

    shared::util::LookupTable<steeringAngle_lut_length> steeringAngle_adc_lut{steeringAngle_lut_data};

    T result = steeringAngle_adc_lut.Evaluate(Angle);
    return result;
}


        
template <typename T>
std::tuple<T,T> tvFactoring(T p_steeringAngle, T tvFactor){    //don't pass by reference (struct or tuple)
    T p_tvFactorLeft;
    T p_tvFactorRight; 

    if (p_steeringAngle > 0){
        p_tvFactorLeft = T(1);
        p_tvFactorRight = tvFactor;
    }
    else if (p_steeringAngle < 0){
        p_tvFactorRight = T(1);
        p_tvFactorLeft = tvFactor;
    }
    else{
        p_tvFactorLeft = T(1);
        p_tvFactorRight = T(1);
    }

    return std::make_tuple(p_tvFactorLeft, p_tvFactorRight);

    //return
}

}
