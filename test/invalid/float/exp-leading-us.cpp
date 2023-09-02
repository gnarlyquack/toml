#include "../../test_common.hpp"


using namespace std;
using namespace toml;


TEST(invalid_float_tests, test_exp_leading_us)
{
    const string toml{"exp-leading-us = 1e_23\n", 23};

    const vector<Error> expected{{ 1, 18, "'_' must separate digits in exponential part of decimal number: 1e_23" },};

    assert_errors(toml, expected);
}
