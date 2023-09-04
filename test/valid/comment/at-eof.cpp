#include "../../test_common.hpp"


using namespace std;
using namespace toml;


TEST(valid_comment_tests, test_at_eof)
{
    const string toml{"# This is a full-line comment\nkey = \"value\" # This is a comment at the end of a line\n", 85};

    const Table expected{{ "key", new StringValue("value") },};

    assert_parsed(toml, expected);
}