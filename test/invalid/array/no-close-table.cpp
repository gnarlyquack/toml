TEST(invalid_array_tests, test_no_close_table)
{
    const string toml{"x = [{ key = 42\n", 16};

    const vector<Error> expected{{ 1, 16, "Unterminated inline table." },{ 2, 1, "Unterminated array." },};

    assert_errors(toml, expected);
}
