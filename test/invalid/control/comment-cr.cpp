#include "../../test_common.hpp"


using namespace std;
using namespace toml;


TEST(invalid_control_tests, test_comment_cr)
{
    const string toml{"comment-cr = \"Carriage return in comment\" # \ra=1\n", 49};

    const vector<Error> expected{{ 1, 45, "Unicode character U+000D not allowed in comment." },};

    assert_errors(toml, expected);
}
