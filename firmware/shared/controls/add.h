#pragma once  // all header files should start with this to prevent re-inclusion

namespace ctrl {  // use the `ctrl` namespace to hold our functions

// Templating means this function will work on any types that support addition.
template <typename T>
T add(T x, T y) {
    // implement your function
    T sum = x + y;
    return sum;
}

}  // namespace ctrl