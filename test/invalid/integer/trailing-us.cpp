#include "../../test_common.hpp"


using namespace std;
using namespace toml;


TEST(invalid_integer_tests, test_trailing_us)
{
    const string toml{"trailing-us = 123_\n", 19};

    const vector<Error> expected{{ 1, 15, "'_' must separate two digits in number: 123_." },};

    assert_errors(toml, expected);
}
