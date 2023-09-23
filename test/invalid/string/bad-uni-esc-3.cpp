TEST(invalid_string_tests, test_bad_uni_esc_3)
{
    const string toml("str = \"val\\U0000000\"\n\n", 22);

    const vector<Error> expected = {
        { 10, 1, 11, "Invalid or incomplete Unicode escape sequence: expected 8 hexadecimal characters but parsed 7." },
    };

    assert_errors(toml, expected);
}
