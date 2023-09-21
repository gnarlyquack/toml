TEST(invalid_control_tests, test_comment_null)
{
    const string toml("comment-null = \"null\" # \000\n", 26);

    const vector<Error> expected = {
        { 24, 1, 25, "Unicode codepoint U+0000 is not allowed." },
    };

    assert_errors(toml, expected);
}
