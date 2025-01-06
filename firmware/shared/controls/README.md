# C++ Controls Functions

## Procedure

1. Create a file for your function. Call it `function.h` (replace "function" with the proper name).
2. Implement your function. See the guiding comments in `add.h`.
3. Write a test file called `function_test.cc`.
4. Write some test cases using the `ASSERT_...` macros described in `testing.h`. See `add_test.cc` for examples.

## Running your tests

You must have Make and G++ installed. This example will use the given `add_test.cc` file.

```bash
make add_test
```

You should see:

```bash
All tests passed
```

If any test fails, you will see

```bash
Assertion failed: <REASON>, file add_test.cc, line 10
```

which means your test fail at line 10.
