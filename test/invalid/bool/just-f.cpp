TEST(invalid_bool_tests, test_just_f)
{
    const string toml("a = f\n", 6);

    const vector<Error> expected = {
        { 4, 1, 5, "Invalid value: f" },
    };

    assert_errors(toml, expected);
}
