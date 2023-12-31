TEST(invalid_control_tests, test_comment_del)
{
    const string toml("comment-del = \"0x7f\" # \177\n", 25);

    const vector<Error> expected = {
        { 23, 1, 24, "Unicode codepoint U+007F is not allowed." },
    };

    assert_errors(toml, expected);
}
