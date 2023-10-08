TEST(invalid_string_tests, test_bad_hex_esc_4)
{
    const string toml("bad-hex-esc-4 = \"\\x 50\"\n", 24);

    const vector<Error> expected = {
        { 17, 1, 18, "Invalid escape sequence: \\x" },
    };

    assert_errors(toml, expected);
}
