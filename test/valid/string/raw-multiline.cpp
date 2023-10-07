TEST(valid_string_tests, test_raw_multiline)
{
    const string toml("# Single ' should be allowed.\noneline = '''This string has a ' quote character.'''\n\n# A newline immediately following the opening delimiter will be trimmed.\nfirstnl = '''\nThis string has a ' quote character.'''\n\n# All other whitespace and newline characters remain intact.\nmultiline = '''\nThis string\nhas ' a quote character\nand more than\none newline\nin it.'''\n\n# Tab character in literal string does not need to be escaped\nmultiline_with_tab = '''First line\n\t Followed by a tab'''\n", 482);

    const Table expected = {{ std::string("firstnl", 7), Value(std::string("This string has a ' quote character.", 36)) },{ std::string("multiline", 9), Value(std::string("This string\nhas ' a quote character\nand more than\none newline\nin it.", 68)) },{ std::string("oneline", 7), Value(std::string("This string has a ' quote character.", 36)) },{ std::string("multiline_with_tab", 18), Value(std::string("First line\n\t Followed by a tab", 30)) }};

    assert_parsed(toml, expected);
}
