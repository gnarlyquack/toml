#include "../../test_common.hpp"


using namespace std;
using namespace toml;


TEST(invalid_control_tests, test_rawstring_us)
{
    const string toml{"rawstring-us = 'null\037'\n", 23};

    const vector<Error> expected{{ 1, 21, "Unicode character U+001F not allowed in string." },};

    assert_errors(toml, expected);
}
