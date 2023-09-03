#include "../../test_common.hpp"


using namespace std;
using namespace toml;


TEST(invalid_float_tests, test_exp_double_e_2)
{
    const string toml{"exp-double-e-2 = 1e2e3\n", 23};

    const vector<Error> expected{{ 1, 20, "Invalid exponential part of decimal number: 1e2e3" },};

    assert_errors(toml, expected);
}
