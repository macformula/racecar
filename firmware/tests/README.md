# Tests

## Separate Build System

On Window, googletest cannot be compiled with the `Unix Makefiles` CMake generator. I spent several hours trying to solve this with no success, so the tests will be compiled separately by the device's native compiler.
