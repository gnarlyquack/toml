#include "../../test_common.hpp"


using namespace std;
using namespace toml;


TEST(invalid_integer_tests, test_us_after_bin)
{
    const string toml{"us-after-bin = 0b_1\n", 20};

    const vector<Error> expected{{ 1, 16, "'_' must separate two digits in number: 0b_1." },};

    assert_errors(toml, expected);
}
