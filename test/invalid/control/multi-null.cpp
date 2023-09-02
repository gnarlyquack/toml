#include "../../test_common.hpp"


using namespace std;
using namespace toml;


TEST(invalid_control_tests, test_multi_null)
{
    const string toml{"multi-null = \"\"\"null\000\"\"\"\n", 25};

    const vector<Error> expected{{ 1, 21, "Unicode character U+0000 not allowed in string." },};

    assert_errors(toml, expected);
}
