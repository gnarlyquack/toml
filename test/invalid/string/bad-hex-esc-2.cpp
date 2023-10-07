TEST(invalid_string_tests, test_bad_hex_esc_2)
{
    const string toml("bad-hex-esc-2 = \"\\xG0\"\n", 23);

    const vector<Error> expected = {
        { 17, 1, 18, "Invalid escape sequence." },
    };

    assert_errors(toml, expected);
}
