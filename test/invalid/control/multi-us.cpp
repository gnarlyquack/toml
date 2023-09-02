#include "../../test_common.hpp"


using namespace std;
using namespace toml;


TEST(invalid_control_tests, test_multi_us)
{
    const string toml{"multi-us = \"\"\"null\037\"\"\"\n", 23};

    const vector<Error> expected{{ 1, 19, "Unicode character U+001F not allowed in string." },};

    assert_errors(toml, expected);
}
