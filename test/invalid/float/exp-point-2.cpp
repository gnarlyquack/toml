#include "../../test_common.hpp"


using namespace std;
using namespace toml;


TEST(invalid_float_tests, test_exp_point_2)
{
    const string toml{"exp-point-2 = 1.e2\n", 19};

    const vector<Error> expected{{ 1, 17, "Missing fractional part of decimal number." },};

    assert_errors(toml, expected);
}
