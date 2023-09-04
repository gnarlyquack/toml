#include "../../test_common.hpp"


using namespace std;
using namespace toml;


TEST(invalid_string_tests, test_bad_byte_escape)
{
    const string toml{"naughty = \"\\xAg\"\n", 17};

    const vector<Error> expected{{ 1, 12, "Invalid escape sequence." },};

    assert_errors(toml, expected);
}
