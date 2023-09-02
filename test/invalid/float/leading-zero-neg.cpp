#include "../../test_common.hpp"


using namespace std;
using namespace toml;


TEST(invalid_float_tests, test_leading_zero_neg)
{
    const string toml{"leading-zero-neg = -03.14\n", 26};

    const vector<Error> expected{{ 1, 20, "Leading zero is not allowed in whole part of decimal number." },};

    assert_errors(toml, expected);
}
