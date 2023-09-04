#include "../../test_common.hpp"


using namespace std;
using namespace toml;


TEST(invalid_float_tests, test_leading_zero_plus)
{
    const string toml{"leading-zero-plus = +03.14\n", 27};

    const vector<Error> expected{{ 1, 22, "Leading zeros are not allowed in whole part of decimal number." },};

    assert_errors(toml, expected);
}
