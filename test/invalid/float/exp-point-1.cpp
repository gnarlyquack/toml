#include "../../test_common.hpp"


using namespace std;
using namespace toml;


TEST(invalid_float_tests, test_exp_point_1)
{
    const string toml{"exp-point-1 = 1e2.3\n", 20};

    const vector<Error> expected{{ 1, 15, "Invalid exponential part of decimal number: 1e2.3" },};

    assert_errors(toml, expected);
}
