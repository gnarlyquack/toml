TEST(invalid_array_tests, test_no_close_table_2)
{
    const string toml("x = [{ key = 42 #\n", 18);

    const vector<Error> expected = {
        { 16, 1, 17, "Missing closing '}' for inline table." },
        { 18, 2,  1, "Missing closing ']' for array." },
    };

    assert_errors(toml, expected);
}
