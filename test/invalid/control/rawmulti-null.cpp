#include "../../test_common.hpp"


using namespace std;
using namespace toml;


TEST(invalid_control_tests, test_rawmulti_null)
{
    const string toml{"rawmulti-null = '''null\000'''\n", 28};

    const vector<Error> expected{{ 1, 24, "Unicode character U+0000 not allowed in string." },};

    assert_errors(toml, expected);
}
