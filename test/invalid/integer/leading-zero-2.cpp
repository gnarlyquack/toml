#include "../../test_common.hpp"


using namespace std;
using namespace toml;


TEST(invalid_integer_tests, test_leading_zero_2)
{
    const string toml{"leading-zero-2 = 00\n", 20};

    const vector<Error> expected{{ 1, 20, "Leading zeros in decimal number not allowed." },};

    assert_errors(toml, expected);
}
