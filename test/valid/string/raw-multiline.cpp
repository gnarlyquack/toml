TEST(valid_string_tests, test_raw_multiline)
{
    const string toml{"# Single ' should be allowed.\noneline = '''This string has a ' quote character.'''\n\n# A newline immediately following the opening delimiter will be trimmed.\nfirstnl = '''\nThis string has a ' quote character.'''\n\n# All other whitespace and newline characters remain intact.\nmultiline = '''\nThis string\nhas ' a quote character\nand more than\none newline\nin it.'''\n\n# Tab character in literal string does not need to be escaped\nmultiline_with_tab = '''First line\n\t Followed by a tab'''\n", 482};

    const Table expected{{ "firstnl", Value(std::string("This string has a ' quote character.")) },{ "multiline", Value(std::string("This string\nhas ' a quote character\nand more than\none newline\nin it.")) },{ "oneline", Value(std::string("This string has a ' quote character.")) },{ "multiline_with_tab", Value(std::string("First line\n\t Followed by a tab")) }};

    assert_parsed(toml, expected);
}
