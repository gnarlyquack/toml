#include "../../test_common.hpp"


using namespace std;
using namespace toml;


TEST(invalid_float_tests, test_leading_point_neg)
{
    const string toml{"leading-point-neg = -.12345\n", 28};

    const vector<Error> expected{{ 1, 22, "Missing whole part of decimal number." },};

    assert_errors(toml, expected);
}
