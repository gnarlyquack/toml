TEST(valid_spec_tests, test_comment_0)
{
    const string toml("# This is a full-line comment\nkey = \"value\"  # This is a comment at the end of a line\nanother = \"# This is not a comment\"\n", 122);

    const Table expected = {{ std::string("another", 7), Value(std::string("# This is not a comment", 23)) },{ std::string("key", 3), Value(std::string("value", 5)) }};

    assert_parsed(toml, expected);
}
