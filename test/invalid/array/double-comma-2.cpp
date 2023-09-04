TEST(invalid_array_tests, test_double_comma_2)
{
    const string toml{"array = [1,2,,]\n\n", 17};

    const vector<Error> expected{{ 1, 14, "Missing value for array." },};

    assert_errors(toml, expected);
}
