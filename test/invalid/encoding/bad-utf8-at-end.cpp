TEST(invalid_encoding_tests, test_bad_utf8_at_end)
{
    const string toml("# There is a 0xda at after the quotes, and no EOL at the end of the file.\n#\n# This is a bit of an edge case: This indicates there should be two bytes\n# (0b1101_1010) but there is no byte to follow because it's the end of the file.\nx = \"\"\"\"\"\"\332", 242);

    const vector<Error> expected = {
        { 241, 5, 11, "Invalid UTF-8: expected a 2-byte Unicode codepoint but encountered invalid or missing bytes after decoding 1 byte(s)." },
    };

    assert_errors(toml, expected);
}
