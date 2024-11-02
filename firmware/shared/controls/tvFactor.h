#pragma once
#include <cmath>
#include <vector>
#include <algorithm>

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
    std::vector<double> breakpoints = {0, 5, 10, 15, 20, 25}; //should datatype be T?
    std::vector<double> tableData = {1, 0.934, 0.87, 0.808, 0.747, 0.683};

    if DI_p_steeringAngle <= breakpoints.front(){
        return tableData.front();
    }
    if DI_p_steeringAngle >= breakpoints.back(){
        return tableData.back();
    }

    int break_size = breakpoints.size();
    int table_size = tableData.size();

    for (int i = 1; i < break_size; i++){
        if (input <= breakpoints[i]) {
            double x1 = breakpoints[i-1]; //should these also be t?
            double x2 = breakpoints[i];
            double y1 = tableData[i-1];
            double y2 = tableData[i];
            double t = (DI_p_steeringAngle - x1) / (x2 - x1); // Calculate position between x1 and x2
            return y1 + t * (y2 - y1);           // Interpolated value
        }

    }
    return breakpoints.back(); //this is if we are higher than the last breakpoint
}

template <typename T>
void tvFactoring(T p_steeringAngle, T tvFactor, T &p_tvFactorLeft, T &p_tvFactorRight){    
    if (p_steeringAngle > 0){
        p_tvFactorLeft = 1;
        p_tvFactorRight = tvFactor;
    }
    else if (p_steeringAngle < 0){
        p_tvFactorRight = 1;
        p_tvFactorLeft = tvFactor;
    }
    else{
        p_tvFactorLeft = 1;
        p_tvFactorRight = 1;
    }
}

}
