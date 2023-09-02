#include "../../test_common.hpp"


using namespace std;
using namespace toml;


TEST(invalid_float_tests, test_trailing_us)
{
    const string toml{"trailing-us = 1.2_\n", 19};

    const vector<Error> expected{{ 1, 15, "'_' must separate digits in fractional part of decimal number: 1.2_" },};

    assert_errors(toml, expected);
}
