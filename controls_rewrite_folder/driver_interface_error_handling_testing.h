/**
 * @author Blake Freer (.com)
 * @brief Provies some helper functions for testing.
 */

#include <cassert>
#include <cmath>
#include <concepts>

const double TOLERANCE = 1e-5;

// Use these for integer types
#define ASSERT_EQ(x, y) assert((x) == (y))
#define ASSERT_NEQ(x, y) assert((x) != (y))

// Use these for float / double types
#define ASSERT_CLOSE(x, y) assert(std::abs((x) - (y)) < (decltype(x))TOLERANCE)
#define ASSERT_NCLOSE(x, y) assert(std::abs((x) - (y)) > (decltype(x))TOLERANCE)