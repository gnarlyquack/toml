#include "../../test_common.hpp"


using namespace std;
using namespace toml;


TEST(invalid_control_tests, test_string_us)
{
    const string toml{"string-us = \"null\037\"\n", 20};

    const vector<Error> expected{{ 1, 18, "Unicode codepoint U+001F is not allowed." },};

    assert_errors(toml, expected);
}
