// don't need `#pragma once` in a .cc / source file

#include "add.h"  // include the header with your function implementation

#include <iostream>

#include "testing.h"

int main() {
    // Write your test cases.

    // Use _CLOSE and _NCLOSE when testing on doubles / floats to avoid floating
    // point precision errors.
    ASSERT_CLOSE(ctrl::add(1.5, 2.5), 4.0);      // 1.5 + 2.5 should be 4.0
    ASSERT_NCLOSE(ctrl::add(0.5f, 0.6f), 1.0f);  // 0.5 + 0.6 should NOT be 1.0

    // Use _EQ and _NEQ on integer types.
    ASSERT_EQ(ctrl::add(-7, 3), -4);
    ASSERT_NEQ(ctrl::add(100, 200), 0);

    // This statement will not be reached if an assert fails.
    std::cout << "All tests passed" << std::endl;
}