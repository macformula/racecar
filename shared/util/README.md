# Util

## `LookupTable`

Evaluate a piecewise function given by an array of key-value pairs. Linearly interpolates between values according to the input key.

The input to `Evaluate()` is clamped between the lowest and highest key.

See [test_lookup_table.cc](test_lookup_table.cc) for an example.
