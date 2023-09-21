TEST(invalid_control_tests, test_bare_formfeed)
{
    const string toml("bare-formfeed = \f\n", 18);

    const vector<Error> expected = {
        { 16, 1, 17, "Unicode codepoint U+000C is not allowed." },
        // TODO: remove this error
        { 16, 1, 17, "Invalid value: \f" },
    };

    assert_errors(toml, expected);
}
