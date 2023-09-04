TEST(invalid_control_tests, test_comment_cr)
{
    const string toml{"comment-cr = \"Carriage return in comment\" # \ra=1\n", 49};

    const vector<Error> expected{{ 1, 45, "Unicode codepoint U+000D is not allowed." },};

    assert_errors(toml, expected);
}
