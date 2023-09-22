TEST(invalid_encoding_tests, test_bad_utf8_in_multiline_literal)
{
    const string toml("# The following line contains an invalid UTF-8 sequence.\nbad = '''\303'''\n", 71);

    const vector<Error> expected = {
        { 66, 2, 10, "Invalid UTF-8 continuation byte: 0x27. Expected a byte in the range of 0x80-0xBF." },
    };

    assert_errors(toml, expected);
}
