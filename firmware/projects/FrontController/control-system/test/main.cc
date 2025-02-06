#include <iostream>

extern void AmkTest();
extern void BmTest();
extern void DiTest();
extern void DiFsmTest();
extern void GovernorTest();
extern void VdTest();

int main() {
    AmkTest();
    BmTest();
    DiTest();
    DiFsmTest();
    GovernorTest();
    VdTest();

    std::cout << "All Control System tests passed!" << std::endl;
    return 0;
}