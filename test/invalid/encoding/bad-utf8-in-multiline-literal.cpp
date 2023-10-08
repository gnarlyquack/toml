TEST(invalid_encoding_tests, test_bad_utf8_in_multiline_literal)
{
    const string toml("# The following line contains an invalid UTF-8 sequence.\nbad = '''\303'''\n", 71);

    const vector<Error> expected = {
        { 66, 2, 10, "Invalid UTF-8: expected a 2-byte Unicode codepoint but encountered invalid or missing bytes after decoding 1 byte(s)." },
    };

    assert_errors(toml, expected);
}
