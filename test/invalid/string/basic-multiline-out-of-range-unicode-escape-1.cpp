#include "../../test_common.hpp"


using namespace std;
using namespace toml;


TEST(invalid_string_tests, test_basic_multiline_out_of_range_unicode_escape_1)
{
    const string toml{"a = \"\"\"\\UFFFFFFFF\"\"\"\n", 21};

    const vector<Error> expected{{ 1, 8, "Invalid Unicode character U+FFFFFFFF." },};

    assert_errors(toml, expected);
}
