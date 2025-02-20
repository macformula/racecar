#include <iostream>

extern void AmkTest();
extern void BmTest();
extern void DiTest();
extern void DiFsmTest();
extern void GovTest();
extern void VdTest();
extern void VdCalcTest();

int main() {
    AmkTest();
    BmTest();
    DiTest();
    DiFsmTest();
    GovTest();
    VdTest();
    VdCalcTest();

    std::cout << "All Control System tests passed!" << std::endl;
    return 0;
}