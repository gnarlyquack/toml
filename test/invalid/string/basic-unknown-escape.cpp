#include "../../test_common.hpp"


using namespace std;
using namespace toml;


TEST(invalid_string_tests, test_basic_unknown_escape)
{
    const string toml{"a = \"\\@\"\n", 9};

    const vector<Error> expected{{ 1, 7, "Invalid escape: '@'." },};

    assert_errors(toml, expected);
}
