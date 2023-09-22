TEST(invalid_encoding_tests, test_bad_utf8_in_string)
{
    const string toml("# The following line contains an invalid UTF-8 sequence.\nbad = \"\303\"\n", 67);

    const vector<Error> expected = {
        { 64, 2, 8, "Invalid UTF-8 continuation byte: 0x22. Expected a byte in the range of 0x80-0xBF." },
    };


    assert_errors(toml, expected);
}
