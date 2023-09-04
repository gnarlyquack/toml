#include "../../test_common.hpp"


using namespace std;
using namespace toml;


TEST(invalid_integer_tests, test_leading_zero_sign_1)
{
    const string toml{"leading-zero-sign-1 = -01\n", 26};

    const vector<Error> expected{{ 1, 24, "Leading zeros are not allowed in decimal number." },};

    assert_errors(toml, expected);
}
