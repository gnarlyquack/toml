TEST(valid_comment_tests, test_at_eof2)
{
    const string toml{"# This is a full-line comment\nkey = \"value\" # This is a comment at the end of a line\n", 85};

    const Table expected{{ "key", Value(std::string("value")) }};

    assert_parsed(toml, expected);
}
