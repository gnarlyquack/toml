#include "../../test_common.hpp"


using namespace std;
using namespace toml;


TEST(invalid_string_tests, test_basic_out_of_range_unicode_escape_2)
{
    const string toml{"a = \"\\U00D80000\"\n", 17};

    const vector<Error> expected{{ 1, 0, "Invalid Unicode character U+D80000." },};

    assert_errors(toml, expected);
}
