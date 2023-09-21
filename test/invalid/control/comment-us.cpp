TEST(invalid_control_tests, test_comment_us)
{
    const string toml("comment-us = \"ctrl-_\" # \037\n", 26);

    const vector<Error> expected = {
        { 24, 1, 25, "Unicode codepoint U+001F is not allowed." },
    };

    assert_errors(toml, expected);
}
