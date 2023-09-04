TEST(invalid_string_tests, test_bad_uni_esc_4)
{
    const string toml{"str = \"val\\U0000\"\n", 18};

    const vector<Error> expected{
        { 1, 11, "Invalid or incomplete Unicode escape sequence: expected 8 hexadecimal characters but parsed 4." },
    };

    assert_errors(toml, expected);
}
