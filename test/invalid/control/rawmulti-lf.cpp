#include "../../test_common.hpp"


using namespace std;
using namespace toml;


TEST(invalid_control_tests, test_rawmulti_lf)
{
    const string toml{"rawmulti-lf = '''null\020'''\n", 26};

    const vector<Error> expected{{ 1, 22, "Unicode character U+0010 not allowed in string." },};

    assert_errors(toml, expected);
}
