TEST(invalid_key_tests, test_without_value_4)
{
    const string toml{"\"key\" = \n", 9};

    const vector<Error> expected{{ 1, 9, "Missing value." },};

    assert_errors(toml, expected);
}
