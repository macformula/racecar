#include <iostream> 

#include "tvFactor.h"

#include "testing.h"

int main (){
    // Use these for float / double types
    // #define ASSERT_CLOSE(x, y) assert(std::abs((x) - (y)) < (decltype(x))TOLERANCE)
    // #define ASSERT_NCLOSE(x, y) assert(std::abs((x) - (y)) > (decltype(x))TOLERANCE)

    using namespace ctrl;

    ASSERT_CLOSE(tvEnable(26.3, 0.2), 26.3);   // Should return 30.0 because DI_b_tvEnable > 0
    ASSERT_CLOSE(tvEnable(21.3, 0.0), 0.0);    // Should return 0.0 because DI_b_tvEnable <= 0
    ASSERT_CLOSE(tvEnable(-20.0, 1.5), -20.0); // Should return -20.0 because DI_b_tvEnable > 0
    ASSERT_CLOSE(tvEnable(12.0, -6.2), 0.0);   // Should return 0.0 because DI_b_tvEnable <= 0

    ASSERT_CLOSE(open_loop_tv_lookUp(5.0), 0.934);
    ASSERT_CLOSE(open_loop_tv_lookUp(10.0), 0.87);
    ASSERT_CLOSE(open_loop_tv_lookUp(13.2), 0.83032);
    ASSERT_CLOSE(open_loop_tv_lookUp(23.3), 0.70476);
    ASSERT_CLOSE(open_loop_tv_lookUp(27.3), 0.683);

    auto [leftFactor1, rightFactor1] = tvFactoring(15.2, 3.6);
    ASSERT_CLOSE(leftFactor1, 1.0); 
    ASSERT_CLOSE(rightFactor1, 3.6);


    auto [leftFactor2, rightFactor2] = tvFactoring(-13.6, 2.2); 
    ASSERT_CLOSE(leftFactor2, 2.2);  
    ASSERT_CLOSE(rightFactor2, 1); 

    auto [leftFactor3, rightFactor3] = tvFactoring(0.0, 6.0); 
    ASSERT_CLOSE(leftFactor3, 1);  
    ASSERT_CLOSE(rightFactor3, 1); 


    std::cout << "All tests passed" << std::endl;
    
    return 0;
}


