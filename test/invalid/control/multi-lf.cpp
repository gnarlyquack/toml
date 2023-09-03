#include "../../test_common.hpp"


using namespace std;
using namespace toml;


TEST(invalid_control_tests, test_multi_lf)
{
    const string toml{"multi-lf = \"\"\"null\020\"\"\"\n", 23};

    const vector<Error> expected{{ 1, 19, "Unicode codepoint U+0010 is not allowed." },};

    assert_errors(toml, expected);
}
