#include "../../test_common.hpp"


using namespace std;
using namespace toml;


TEST(invalid_float_tests, test_exp_trailing_us)
{
    const string toml{"exp-trailing-us = 1e_23_\n", 25};

    const vector<Error> expected{{ 1, 21, "'_' must separate digits in exponential part of decimal number: 1e_23_" },};

    assert_errors(toml, expected);
}
