TEST(valid_string_tests, test_escape_tricky)
{
    const string toml("end_esc = \"String does not end here\\\" but ends here\\\\\"\nlit_end_esc = 'String ends here\\'\n\nmultiline_unicode = \"\"\"\n\\u00a0\"\"\"\n\nmultiline_not_unicode = \"\"\"\n\\\\u0041\"\"\"\n\nmultiline_end_esc = \"\"\"When will it end? \\\"\"\"...\"\"\\\" should be here\\\"\"\"\"\n\nlit_multiline_not_unicode = '''\n\\u007f'''\n\nlit_multiline_end = '''There is no escape\\'''\n", 328);

    const Table expected = {{ std::string("end_esc", 7), Value(std::string("String does not end here\" but ends here\\", 40)) },{ std::string("lit_end_esc", 11), Value(std::string("String ends here\\", 17)) },{ std::string("lit_multiline_end", 17), Value(std::string("There is no escape\\", 19)) },{ std::string("lit_multiline_not_unicode", 25), Value(std::string("\\u007f", 6)) },{ std::string("multiline_end_esc", 17), Value(std::string("When will it end? \"\"\"...\"\"\" should be here\"", 43)) },{ std::string("multiline_not_unicode", 21), Value(std::string("\\u0041", 6)) },{ std::string("multiline_unicode", 17), Value(std::string("\u00A0", 2)) }};

    assert_parsed(toml, expected);
}
