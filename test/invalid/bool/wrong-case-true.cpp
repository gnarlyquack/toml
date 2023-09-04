TEST(invalid_bool_tests, test_wrong_case_true)
{
    const string toml{"a = TRUE\n", 9};

    const vector<Error> expected{{ 1, 5, "Invalid value: TRUE" },};

    assert_errors(toml, expected);
}
