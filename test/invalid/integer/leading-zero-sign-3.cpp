#include "../../test_common.hpp"


using namespace std;
using namespace toml;


TEST(invalid_integer_tests, test_leading_zero_sign_3)
{
    const string toml{"leading-zero-sign-3 = +0_1\n", 27};

    const vector<Error> expected{{ 1, 27, "Leading zeros in decimal number not allowed." },};

    assert_errors(toml, expected);
}
