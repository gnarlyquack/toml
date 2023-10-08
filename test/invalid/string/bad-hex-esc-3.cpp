TEST(invalid_string_tests, test_bad_hex_esc_3)
{
    const string toml("bad-hex-esc-3 = \"\\x\"\n", 21);

    const vector<Error> expected = {
        { 17, 1, 18, "Invalid escape sequence: \\x" },
    };

    assert_errors(toml, expected);
}
