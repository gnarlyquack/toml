#include "../../test_common.hpp"


using namespace std;
using namespace toml;


TEST(valid_string_tests, test_nl)
{
    const string toml{"nl_mid = \"val\\nue\"\nnl_end = \"\"\"value\\n\"\"\"\n\nlit_nl_end = '''value\\n'''\nlit_nl_mid = 'val\\nue'\nlit_nl_uni = 'val\\ue'\n", 115};

    const Table expected{{ "lit_nl_end", new StringValue("value\\n") },{ "lit_nl_mid", new StringValue("val\\nue") },{ "lit_nl_uni", new StringValue("val\\ue") },{ "nl_end", new StringValue("value\n") },{ "nl_mid", new StringValue("val\nue") },};

    assert_parsed(toml, expected);
}
