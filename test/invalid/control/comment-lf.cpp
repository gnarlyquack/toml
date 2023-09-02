#include "../../test_common.hpp"


using namespace std;
using namespace toml;


TEST(invalid_control_tests, test_comment_lf)
{
    const string toml{"comment-lf = \"ctrl-P\" # \020\n", 26};

    const vector<Error> expected{{ 1, 25, "Unicode character U+0010 not allowed in comment." },};

    assert_errors(toml, expected);
}
