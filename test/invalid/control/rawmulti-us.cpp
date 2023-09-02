#include "../../test_common.hpp"


using namespace std;
using namespace toml;


TEST(invalid_control_tests, test_rawmulti_us)
{
    const string toml{"rawmulti-us = '''null\037'''\n", 26};

    const vector<Error> expected{{ 1, 22, "Unicode character U+001F not allowed in string." },};

    assert_errors(toml, expected);
}
