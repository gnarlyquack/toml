TEST(valid_spec_tests, test_string_0)
{
    const string toml("str = \"I'm a string. \\\"You can quote me\\\". Name\\tJos\\u00E9\\nLocation\\tSF.\"\n", 75);

    const Table expected = {{ std::string("str", 3), Value(std::string("I'm a string. \"You can quote me\". Name\tJosé\nLocation\tSF.", 57)) }};

    assert_parsed(toml, expected);
}
