TEST(invalid_control_tests, test_comment_lf)
{
    const string toml("comment-lf = \"ctrl-P\" # \020\n", 26);

    const vector<Error> expected = {
        { 24, 1, 25, "Unicode codepoint U+0010 is not allowed." },
    };

    assert_errors(toml, expected);
}
