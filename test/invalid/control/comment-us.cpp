#include "../../test_common.hpp"


using namespace std;
using namespace toml;


TEST(invalid_control_tests, test_comment_us)
{
    const string toml{"comment-us = \"ctrl-_\" # \037\n", 26};

    const vector<Error> expected{{ 1, 25, "Unicode character U+001F not allowed in comment." },};

    assert_errors(toml, expected);
}
