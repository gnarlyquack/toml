#include "../../test_common.hpp"


using namespace std;
using namespace toml;


TEST(invalid_float_tests, test_us_after_point)
{
    const string toml{"us-after-point = 1._2\n", 22};

    const vector<Error> expected{{ 1, 20, "'_' must separate digits in fractional part of decimal number: 1._2" },};

    assert_errors(toml, expected);
}
