TEST(invalid_string_tests, test_bad_hex_esc_1)
{
    const string toml("bad-hex-esc-1 = \"\\x0g\"\n", 23);

    const vector<Error> expected = {
        { 17, 1, 18, "Invalid escape sequence: \\x" },
    };

    assert_errors(toml, expected);
}
