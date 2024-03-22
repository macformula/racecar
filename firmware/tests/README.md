# Tests

## Writing Tests

We use `googletest` as the testing framework. See the [GoogleTest Primer](https://google.github.io/googletest/primer.html) for instructions.

A "Tests" C/C++ configuration for VSCode has been created to provide include paths for `googletest`.

## Folder structure

Tests are defined in the `test/firmware` folder. When testing code from a firmware script, use the same file path under as under `firmware` and append `_test.cc` to it.

For example, to test `firmware/shared/code.cc`, write a test file called `firmware/tests/firmware/shared/code_test.cc`.

## Compiling and Running Tests

A Makefile has been prepared to help. Run `make` from within `firmware/tests`.

The test output will be printed to the screen.

If this is your first time running tests, you will need to initialize the `googletest` submodule. Run the following command from within `firmware/tests`

```bash
git submodule update --init googletest/
```

## Limitations

On Window, googletest cannot be compiled with the `Unix Makefiles` CMake generator. I spent several hours trying to solve this with no success, so the tests will be compiled separately by the device's native compiler.

- This means that, for now, we cannot test code using the platform-specific compilers. this mostly limits our testing to `shared` code until this is fixed.

In a similar manner, since `tests` uses a separate build system, it cannot rely on `.cc` files from outside of `tests`; it can only include headers from outside. This would be fixed if the `tests` target was compiled in the regular CMakeLists.txt.
