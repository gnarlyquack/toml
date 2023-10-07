TEST(valid_comment_tests, test_at_eof)
{
    const string toml("# This is a full-line comment\nkey = \"value\" # This is a comment at the end of a line\n", 85);

    const Table expected = {{ std::string("key", 3), Value(std::string("value", 5)) }};

    assert_parsed(toml, expected);
}
