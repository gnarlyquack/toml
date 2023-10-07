TEST(valid_string_tests, test_nl)
{
    const string toml("nl_mid = \"val\\nue\"\nnl_end = \"\"\"value\\n\"\"\"\n\nlit_nl_end = '''value\\n'''\nlit_nl_mid = 'val\\nue'\nlit_nl_uni = 'val\\ue'\n", 115);

    const Table expected = {{ std::string("lit_nl_end", 10), Value(std::string("value\\n", 7)) },{ std::string("lit_nl_mid", 10), Value(std::string("val\\nue", 7)) },{ std::string("lit_nl_uni", 10), Value(std::string("val\\ue", 6)) },{ std::string("nl_end", 6), Value(std::string("value\n", 6)) },{ std::string("nl_mid", 6), Value(std::string("val\nue", 6)) }};

    assert_parsed(toml, expected);
}
