#include "../../test_common.hpp"


using namespace std;
using namespace toml;


TEST(invalid_float_tests, test_leading_zero)
{
    const string toml{"leading-zero = 03.14\n", 21};

    const vector<Error> expected{{ 1, 16, "Leading zero is not allowed in whole part of decimal number." },};

    assert_errors(toml, expected);
}
