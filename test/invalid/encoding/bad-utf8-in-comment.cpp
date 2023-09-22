TEST(invalid_encoding_tests, test_bad_utf8_in_comment)
{
    const string toml("# \303\n", 4);

    const vector<Error> expected = {
        { 2, 1, 3, "Invalid UTF-8 continuation byte: 0x0A. Expected a byte in the range of 0x80-0xBF." },
    };

    assert_errors(toml, expected);
}
