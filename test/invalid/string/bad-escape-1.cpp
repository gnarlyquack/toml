#include "../../test_common.hpp"


using namespace std;
using namespace toml;


TEST(invalid_string_tests, test_bad_escape_1)
{
    const string toml{"invalid-escape = \"This string has a bad \\a escape character.\"\n", 62};

    const vector<Error> expected{{ 1, 42, "Invalid escape: 'a'." },};

    assert_errors(toml, expected);
}
