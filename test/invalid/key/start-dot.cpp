TEST(invalid_key_tests, test_start_dot)
{
    const string toml(".key = 1\n", 9);

    const vector<Error> expected = {
        { 0, 1, 1, "Invalid expression: .key = 1" },
    };

    assert_errors(toml, expected);
}
