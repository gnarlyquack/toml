#include "../../test_common.hpp"


using namespace std;
using namespace toml;


TEST(valid_string_tests, test_raw_multiline)
{
    const string toml{"# Single ' should be allowed.\noneline = '''This string has a ' quote character.'''\n\n# A newline immediately following the opening delimiter will be trimmed.\nfirstnl = '''\nThis string has a ' quote character.'''\n\n# All other whitespace and newline characters remain intact.\nmultiline = '''\nThis string\nhas ' a quote character\nand more than\none newline\nin it.'''\n\n# Tab character in literal string does not need to be escaped\nmultiline_with_tab = '''First line\n\t Followed by a tab'''\n", 482};

    const Table expected{{ "firstnl", new StringValue("This string has a ' quote character.") },{ "multiline", new StringValue("This string\nhas ' a quote character\nand more than\none newline\nin it.") },{ "oneline", new StringValue("This string has a ' quote character.") },{ "multiline_with_tab", new StringValue("First line\n\t Followed by a tab") },};

    assert_parsed(toml, expected);
}
