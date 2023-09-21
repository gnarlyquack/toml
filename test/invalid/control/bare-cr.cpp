TEST(invalid_control_tests, test_bare_cr)
{
    const string toml("# The following line contains a single carriage return control character\r\n\r", 75);

    const vector<Error> expected = {
        { 74, 2, 1, "Unicode codepoint U+000D is not allowed." },
    };

    assert_errors(toml, expected);
}
