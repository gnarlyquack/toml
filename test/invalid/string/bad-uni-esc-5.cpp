TEST(invalid_string_tests, test_bad_uni_esc_5)
{
    const string toml{"str = \"val\\Ugggggggg\"\n", 22};

    const vector<Error> expected{
        { 1, 11, "Invalid or incomplete Unicode escape sequence: expected 8 hexadecimal characters but parsed 0." },
    };

    assert_errors(toml, expected);
}
