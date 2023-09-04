#include "../../test_common.hpp"


using namespace std;
using namespace toml;


TEST(invalid_integer_tests, test_trailing_us_hex)
{
    const string toml{"trailing-us-hex = 0x1_\n", 23};

    const vector<Error> expected{{ 1, 21, "'_' must separate digits in hexadecimal number: 0x1_" },};

    assert_errors(toml, expected);
}
