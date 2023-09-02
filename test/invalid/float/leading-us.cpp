#include "../../test_common.hpp"


using namespace std;
using namespace toml;


TEST(invalid_float_tests, test_leading_us)
{
    const string toml{"leading-us = _1.2\n", 18};

    const vector<Error> expected{{ 1, 14, "Invalid value: _1.2" },};

    assert_errors(toml, expected);
}
