#include "../../test_common.hpp"


using namespace std;
using namespace toml;


TEST(valid_string_tests, test_multiline_escaped_crlf)
{
    const string toml{"# The following line should be an unescaped backslash followed by a Windows\r\n# newline sequence (\"\\r\\n\")\r\n0=\"\"\"\\\r\n\"\"\"\r\n", 119};

    const Table expected{{ "0", new StringValue("") },};

    assert_parsed(toml, expected);
}