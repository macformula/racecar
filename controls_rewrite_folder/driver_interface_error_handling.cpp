#include <iostream>
#include <cmath>
//everson shen and aleeza ali zar



double potentiometerProcessing(double fractionIn){

    if (fractionIn >= 0 && fractionIn <= 1){
        return fractionIn;
    }
    else{
        return 0;
    }
}

bool b_DriverInterfaceError(double DI_V_AccelPedalPos1, double DI_V_AccelPedalPos2, double DI_FrontBrakePressure, double DI_RearBrakePressure, double DI_V_RawSteeringAngle){


    double PedalPos1 = potentiometerProcessing(DI_V_AccelPedalPos1);
    double PedalPos2 = potentiometerProcessing(DI_V_AccelPedalPos2);
    double FrontBrakePressure = potentiometerProcessing(DI_FrontBrakePressure);
    double RearBrakePressure = potentiometerProcessing(DI_RearBrakePressure);
    double RawSteeringAngle = potentiometerProcessing(DI_V_RawSteeringAngle);

    return ((PedalPos1 == 0) || (PedalPos2 == 0) || (FrontBrakePressure == 0) || (RearBrakePressure == 0) || (RawSteeringAngle == 0) || (std::abs(DI_V_AccelPedalPos1 - DI_V_AccelPedalPos2) > 0.1));
    //true -> there's an error
    //false -> no error 
}


int main() {
    // Example usage
    double DI_V_AccelPedalPos1 = 0.8, DI_V_AccelPedalPos2 = 0.9;
    double DI_FrontBrakePressure = 2, DI_RearBrakePressure = 0.7;
    double DI_V_RawSteeringAngle = 0.5; //test case

    bool driverError = b_DriverInterfaceError(
        DI_V_AccelPedalPos1, DI_V_AccelPedalPos2, 
        DI_FrontBrakePressure, DI_RearBrakePressure, 
        DI_V_RawSteeringAngle
    );

    std::cout << "Driver Interface Error: " << (driverError ? "Yes" : "No") << std::endl;

    return 0;
}