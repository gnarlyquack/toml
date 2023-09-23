TEST(invalid_string_tests, test_bad_uni_esc_2)
{
    const string toml("str = \"val\\Ux\"\n", 15);

    const vector<Error> expected = {
        { 10, 1, 11, "Invalid or incomplete Unicode escape sequence: expected 8 hexadecimal characters but parsed 0." },
    };

    assert_errors(toml, expected);
}
