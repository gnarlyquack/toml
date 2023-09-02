#include "../../test_common.hpp"


using namespace std;
using namespace toml;


TEST(invalid_string_tests, test_basic_multiline_unknown_escape)
{
    const string toml{"a = \"\"\"\\@\"\"\"\n", 13};

    const vector<Error> expected{{ 1, 9, "Invalid escape: '@'." },};

    assert_errors(toml, expected);
}
