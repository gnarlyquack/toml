TEST(valid_string_tests, test_escaped_escape)
{
    const string toml{"answer = \"\\\\x64\"\n", 17};

    const Table expected{{ "answer", new StringValue("\\x64") },};

    assert_parsed(toml, expected);
}
