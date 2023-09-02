#include "../../test_common.hpp"


using namespace std;
using namespace toml;


TEST(invalid_control_tests, test_rawstring_null)
{
    const string toml{"rawstring-null = 'null\000'\n", 25};

    const vector<Error> expected{{ 1, 23, "Unicode character U+0000 not allowed in string." },};

    assert_errors(toml, expected);
}
