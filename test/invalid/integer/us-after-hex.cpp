#include "../../test_common.hpp"


using namespace std;
using namespace toml;


TEST(invalid_integer_tests, test_us_after_hex)
{
    const string toml{"us-after-hex = 0x_1\n", 20};

    const vector<Error> expected{{ 1, 18, "'_' must separate digits in hexadecimal number: 0x_1" },};

    assert_errors(toml, expected);
}
