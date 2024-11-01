#pragma once
#include <cmath>
#include <vector>

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
    std::vector<double> breakpoints = {0, 5, 10, 15, 20, 25};
    std::vector<double> tableData = {1, 0.934, 0.87, 0.808, 0.747, 0.683};

    


}

}
