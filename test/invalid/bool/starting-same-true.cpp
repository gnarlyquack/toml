TEST(invalid_bool_tests, test_starting_same_true)
{
    const string toml("a = truer\n", 10);

    const vector<Error> expected = {
        { 4, 1, 5, "Invalid value: truer" },
    };

    assert_errors(toml, expected);
}
