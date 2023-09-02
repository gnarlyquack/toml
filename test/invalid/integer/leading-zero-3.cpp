#include "../../test_common.hpp"


using namespace std;
using namespace toml;


TEST(invalid_integer_tests, test_leading_zero_3)
{
    const string toml{"leading-zero-3 = 0_0\n", 21};

    const vector<Error> expected{{ 1, 21, "Leading zeros in decimal number not allowed." },};

    assert_errors(toml, expected);
}
