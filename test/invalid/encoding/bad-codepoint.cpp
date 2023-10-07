TEST(invalid_encoding_tests, test_bad_codepoint)
{
    const string toml("# Invalid codepoint U+D800 : \355\240\200\n", 33);

    const vector<Error> expected = {
        { 29, 1, 30, "Unicode codepoint U+D800 is not allowed." },
    };

    assert_errors(toml, expected);
}
