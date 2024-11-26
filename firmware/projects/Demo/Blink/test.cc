#include <cassert>
#include <iostream>
#include "main.cc" // Include or import the function to test directly

void test_add() {
    assert(add(2, 3) == 5);
    assert(add(-1, 1) == 0);
    assert(add(0, 0) == 0);
}

int main() {
    test_add();
    std::cout << "All tests passed!" << std::endl;
    return 0;
}