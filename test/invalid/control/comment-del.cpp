#include "../../test_common.hpp"


using namespace std;
using namespace toml;


TEST(invalid_control_tests, test_comment_del)
{
    const string toml{"comment-del = \"0x7f\" # \177\n", 25};

    const vector<Error> expected{{ 1, 24, "Unicode character U+007F not allowed in comment." },};

    assert_errors(toml, expected);
}
