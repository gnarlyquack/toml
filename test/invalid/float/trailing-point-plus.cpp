#include "../../test_common.hpp"


using namespace std;
using namespace toml;


TEST(invalid_float_tests, test_trailing_point_plus)
{
    const string toml{"trailing-point-plus = +1.\n", 26};

    const vector<Error> expected{{ 1, 26, "Missing fractional part of decimal number." },};

    assert_errors(toml, expected);
}
