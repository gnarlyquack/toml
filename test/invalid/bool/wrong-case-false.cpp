TEST(invalid_bool_tests, test_wrong_case_false)
{
    const string toml("b = FALSE\n", 10);

    const vector<Error> expected = {
        { 4, 1, 5, "Invalid value: FALSE" },
    };

    assert_errors(toml, expected);
}
