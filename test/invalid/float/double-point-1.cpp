#include "../../test_common.hpp"


using namespace std;
using namespace toml;


TEST(invalid_float_tests, test_double_point_1)
{
    const string toml{"double-point-1 = 0..1\n", 22};

    const vector<Error> expected{{ 1, 18, "Invalid fractional part of decimal number: 0..1" },};

    assert_errors(toml, expected);
}
