TEST(invalid_bool_tests, test_almost_true_with_extra)
{
    const string toml("a = truthy\n", 11);

    const vector<Error> expected = {
        { 4, 1, 5, "Invalid value: truthy" },
    };

    assert_errors(toml, expected);
}
