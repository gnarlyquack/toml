TEST(invalid_bool_tests, test_mixed_case)
{
    const string toml{"valid = False\n", 14};

    const vector<Error> expected{{ 1, 9, "Invalid value: False" },};

    assert_errors(toml, expected);
}
