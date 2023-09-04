#include "../../test_common.hpp"


using namespace std;
using namespace toml;


TEST(invalid_integer_tests, test_leading_us_bin)
{
    const string toml{"leading-us-bin = _0o1\n", 22};

    const vector<Error> expected{{ 1, 18, "Invalid value: _0o1" },};

    assert_errors(toml, expected);
}
