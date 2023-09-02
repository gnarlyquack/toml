#include "../../test_common.hpp"


using namespace std;
using namespace toml;


TEST(invalid_control_tests, test_string_null)
{
    const string toml{"string-null = \"null\000\"\n", 22};

    const vector<Error> expected{{ 1, 20, "Unicode character U+0000 not allowed in string." },};

    assert_errors(toml, expected);
}
