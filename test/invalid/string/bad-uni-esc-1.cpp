TEST(invalid_string_tests, test_bad_uni_esc_1)
{
    const string toml("str = \"val\\ue\"\n", 15);

    const vector<Error> expected = {
        { 10, 1, 11, "Invalid or incomplete Unicode escape sequence: expected 4 hexadecimal characters but found 1." },
    };

    assert_errors(toml, expected);
}
