TEST(invalid_array_tests, test_double_comma_1)
{
    const string toml{"array = [1,,2]\n", 15};

    const vector<Error> expected{{ 1, 12, "Missing value for array." },};

    assert_errors(toml, expected);
}
