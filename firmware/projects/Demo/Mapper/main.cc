/// @author Blake Freer
/// @date 2024-12-28

#include <format>
#include <iostream>  // for demo only, will not work on other platforms

#include "shared/util/mappers/clamper.hpp"
#include "shared/util/mappers/constant.hpp"
#include "shared/util/mappers/identity.hpp"
#include "shared/util/mappers/linear_map.hpp"
#include "shared/util/mappers/lookup_table.hpp"
#include "shared/util/mappers/mapper.hpp"
#include "shared/util/mappers/quadratic_map.hpp"

void expect_eq(double actual, double expected) {
    std::cout << std::format("Expected {:.2f}, got: {:.2f}", expected, actual);
    if (std::abs(expected - actual) > 0.01) {
        std::cout << "  FAIL";
    }
    std::cout << std::endl;
}

int main(void) {
    {
        std::cout << "Linear Map" << std::endl;
        shared::util::LinearMap<double> f{10., 3.};
        expect_eq(f.Evaluate(7), 73);
    }

    {
        std::cout << "Quadratic Map" << std::endl;
        shared::util::QuadraticMap<double> f{1., 0., 5.};
        expect_eq(f.Evaluate(0.), 5);
        expect_eq(f.Evaluate(1.), 6);
        expect_eq(f.Evaluate(2.), 9);
    }

    {
        std::cout << "Clamper" << std::endl;
        shared::util::Clamper<double> f{0., 10.};
        expect_eq(f.Evaluate(-10.), 0);
        expect_eq(f.Evaluate(3.), 3);
        expect_eq(f.Evaluate(100.), 10);
    }

    {
        std::cout << "Lookup Table" << std::endl;
        const double lut_data[][2] = {{-2., 4.}, {-1., 1.}, {0., 0.},
                                      {1., 1.},  {2., 4.},  {3., 9.}};

        constexpr int lut_length = (sizeof(lut_data)) / (sizeof(lut_data[0]));
        shared::util::LookupTable<lut_length, double> f{lut_data};

        expect_eq(f.Evaluate(-1.5), 2.5);
        expect_eq(f.Evaluate(0.5), 0.5);
        expect_eq(f.Evaluate(2.), 4);
        expect_eq(f.Evaluate(4.), 9);   // (above highest key)
        expect_eq(f.Evaluate(-4.), 4);  // (below lowest key)
    }

    {
        std::cout << "Identity" << std::endl;
        shared::util::IdentityMap<double> f;
        expect_eq(f.Evaluate(6.), 6);
    }

    {
        std::cout << "Constant" << std::endl;
        shared::util::ConstantMap<double> f{42.};
        expect_eq(f.Evaluate(0.5), 42);
        expect_eq(f.Evaluate(10.), 42);
    }

    {
        std::cout << "Composite Map" << std::endl;

        shared::util::LinearMap<double> cel_to_fahr{1.8, 32.};
        shared::util::Clamper<double> limiter{32., 212.};

        shared::util::CompositeMap<double> f{cel_to_fahr, limiter};

        expect_eq(f.Evaluate(20.), 68);    // 20 C = 68 F
        expect_eq(f.Evaluate(-10.), 32);   // -10 C = 14 F, clamped to 32 F
        expect_eq(f.Evaluate(100.), 212);  // 100 C = 212 F
        expect_eq(f.Evaluate(101.), 212);  // 101 C = 213.8 F, clamped to 212 F
    }

    return 0;
}