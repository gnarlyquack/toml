TEST(invalid_array_tests, test_no_close_table)
{
    const string toml("x = [{ key = 42\n", 16);

    const vector<Error> expected = {
        { 15, 1, 16, "Missing closing '}' for inline table." },
        { 16, 2, 1, "Missing closing ']' for array." },
    };

    assert_errors(toml, expected);
}
