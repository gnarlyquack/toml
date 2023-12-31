TEST(invalid_string_tests, test_bad_byte_escape)
{
    const string toml("naughty = \"\\xAg\"\n", 17);

    const vector<Error> expected = {
        { 11, 1, 12, "Invalid escape sequence: \\x" },
    };

    assert_errors(toml, expected);
}
