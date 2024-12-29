# Mappers

Utility functions for evaluating and composing math equations.

You can run the sample code with the `Demo/Mapper` project [[link]](../../../projects/Demo/Mapper/README.md).

## The Abstract `Mapper` Class

A `Mapper` subclass can implements a mathematical function by overriding the `Evaluate(x)` method. There are several built-in subclasses.

## `LinearMap`

$f(x)=mx+b$

Parameterized by `(m, b)`.

```c++
#include "shared/util/mappers/linear_map.hpp"

shared::util::LinearMap<double> f{10., 3.};
expect_eq(f.Evaluate(7), 73);
```

## `QuadraticMap`

$f(x)=ax^2+bx+c$

Parameterized by `(a, b, c)`.

```c++
#include "shared/util/mappers/quadratic_map.hpp"

shared::util::QuadraticMap<double> f{1., 0., 5.};
expect_eq(f.Evaluate(0.), 5);
expect_eq(f.Evaluate(1.), 6);
expect_eq(f.Evaluate(2.), 9);
```

## `Clamper`

Restrict a value between a lower and upper bound.

Parameterized by `(lower, upper)`.

```c++
#include "shared/util/mappers/clamper.hpp"

shared::util::Clamper<double> f{0., 10.};
expect_eq(f.Evaluate(-10.), 0);
expect_eq(f.Evaluate(3.), 3);
expect_eq(f.Evaluate(100.), 10);
```

## `LookupTable`

Evaluate a piecewise function given by an array of key-value pairs.

The input to `Evaluate()` is clamped between the lowest and highest key.

```c++
#include "shared/util/mappers/lookup_table.hpp"

// Define the lookup table key-values.
// The keys must be sorted in increasing order.
// This example approximates x^2 from -2 to +3
const double lut_data[][2] = {
    {-2., 4.},
    {-1., 1.},
    {0., 0.},
    {1., 1.},
    {2., 4.},
    {3., 9.}
};

constexpr int lut_length = (sizeof(lut_data)) / (sizeof(lut_data[0]));
shared::util::LookupTable<lut_length, double> f{lut_data};

expect_eq(f.Evaluate(-1.5), 2.5);
expect_eq(f.Evaluate(0.5), 0.5);
expect_eq(f.Evaluate(2.), 4);
expect_eq(f.Evaluate(4.), 9);   // (above highest key)
expect_eq(f.Evaluate(-4.), 4);  // (below lowest key)
```

## `IdentityMap`

$f(x)=x$

No parameters. This class is needed when another class/function expects a `Mapper` as an input, but the desired input to just return the input.

```c++
#include "shared/util/mappers/identity.hpp"

shared::util::IdentityMap<double> f;
expect_eq(f.Evaluate(6.), 6);
```

## `ConstantMap`

$f(x)=c$

Parameterized by `(c)`. Useful as a placeholder.

```c++
#include "shared/util/mappers/constant.hpp"

shared::util::ConstantMap<double> f{42.};
expect_eq(f.Evaluate(0.5), 42);
expect_eq(f.Evaluate(10.), 42);
```

## `CompositeMap`

$f(x) = h(g(x))$

Parameterized by _mappers_ `(g, h)`. __Note the order:__ inner function is passed first.

Composes two maps. Used when a class/function requires a `Mapper` input but the desired behaviour cannot be expressed with a single `Mapper`, so two must be composed.

For example, to convert Celsius to Fahrenheit, but then clamp the output between 32 degF and 212 degF, you could compose `g(x) = LinearMap` and `h(x)=Clamper`.

```c++
#include "shared/utils/mappers/mapper.hpp" // Defines CompositeMap
#include "shared/utils/mappers/linear_map.hpp"
#include "shared/utils/mappers/clamper.hpp"

shared::util::LinearMap<double> cel_to_fahr{1.8, 32.};
shared::util::Clamper<double> limiter{32., 212.};

shared::util::CompositeMap<double> f{cel_to_fahr, limiter};

expect_eq(f.Evaluate(20.), 68);    // 20 C = 68 F
expect_eq(f.Evaluate(-10.), 32);   // -10 C = 14 F, clamped to 32 F
expect_eq(f.Evaluate(100.), 212);  // 100 C = 212 F
expect_eq(f.Evaluate(101.), 212);  // 101 C = 213.8 F, clamped to 212 F
```
