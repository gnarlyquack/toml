TEST(invalid_datetime_tests, test_trailing_t)
{
    const string toml("# Date cannot end with trailing T\nd = 2006-01-30T\n", 50);

    const vector<Error> expected = {
        { 38, 2, 5, "Invalid value: 2006-01-30T" },
    };

    assert_errors(toml, expected);
}
