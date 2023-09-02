#include "../../test_common.hpp"


using namespace std;
using namespace toml;


TEST(invalid_control_tests, test_comment_null)
{
    const string toml{"comment-null = \"null\" # \000\n", 26};

    const vector<Error> expected{{ 1, 25, "Unicode character U+0000 not allowed in comment." },};

    assert_errors(toml, expected);
}
