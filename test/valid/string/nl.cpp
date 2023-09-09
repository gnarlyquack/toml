TEST(valid_string_tests, test_nl)
{
    const string toml{"nl_mid = \"val\\nue\"\nnl_end = \"\"\"value\\n\"\"\"\n\nlit_nl_end = '''value\\n'''\nlit_nl_mid = 'val\\nue'\nlit_nl_uni = 'val\\ue'\n", 115};

    const Table expected{{ "lit_nl_end", Value(std::string("value\\n")) },{ "lit_nl_mid", Value(std::string("val\\nue")) },{ "lit_nl_uni", Value(std::string("val\\ue")) },{ "nl_end", Value(std::string("value\n")) },{ "nl_mid", Value(std::string("val\nue")) }};

    assert_parsed(toml, expected);
}
