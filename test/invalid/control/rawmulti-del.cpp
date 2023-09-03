#include "../../test_common.hpp"


using namespace std;
using namespace toml;


TEST(invalid_control_tests, test_rawmulti_del)
{
    const string toml{"rawmulti-del = '''null\177'''\n", 27};

    const vector<Error> expected{{ 1, 23, "Unicode codepoint U+007F is not allowed." },};

    assert_errors(toml, expected);
}
