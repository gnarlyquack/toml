#include "../../test_common.hpp"


using namespace std;
using namespace toml;


TEST(invalid_string_tests, test_multiline_bad_escape_1)
{
    const string toml{"k = \"\"\"t\\a\"\"\"\n\n", 15};

    const vector<Error> expected{{ 1, 10, "Invalid escape: 'a'." },};

    assert_errors(toml, expected);
}
