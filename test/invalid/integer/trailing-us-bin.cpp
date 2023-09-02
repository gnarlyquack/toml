#include "../../test_common.hpp"


using namespace std;
using namespace toml;


TEST(invalid_integer_tests, test_trailing_us_bin)
{
    const string toml{"trailing-us-bin = 0b1_\n", 23};

    const vector<Error> expected{{ 1, 19, "'_' must separate two digits in number: 0b1_." },};

    assert_errors(toml, expected);
}
