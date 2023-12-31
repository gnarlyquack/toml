TEST(invalid_key_tests, test_two_equals)
{
    const string toml("key= = 1\n", 9);

    const vector<Error> expected = {
        { 5, 1, 6, "Invalid value: = 1" },
    };

    assert_errors(toml, expected);
}
