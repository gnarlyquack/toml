TEST(invalid_encoding_tests, test_bad_utf8_in_comment)
{
    const string toml("# \303\n", 4);

    const vector<Error> expected = {
        { 2, 1, 3, "Invalid UTF-8: expected a 2-byte Unicode codepoint but encountered invalid or missing bytes after decoding 1 byte(s)." },
    };

    assert_errors(toml, expected);
}
