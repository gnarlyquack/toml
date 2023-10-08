TEST(invalid_encoding_tests, test_bad_utf8_in_string)
{
    const string toml("# The following line contains an invalid UTF-8 sequence.\nbad = \"\303\"\n", 67);

    const vector<Error> expected = {
        { 64, 2, 8, "Invalid UTF-8: expected a 2-byte Unicode codepoint but encountered invalid or missing bytes after decoding 1 byte(s)." },
    };

    assert_errors(toml, expected);
}
