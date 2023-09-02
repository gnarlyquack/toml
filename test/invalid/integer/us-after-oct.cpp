#include "../../test_common.hpp"


using namespace std;
using namespace toml;


TEST(invalid_integer_tests, test_us_after_oct)
{
    const string toml{"us-after-oct = 0o_1\n", 20};

    const vector<Error> expected{{ 1, 16, "'_' must separate two digits in number: 0o_1." },};

    assert_errors(toml, expected);
}
