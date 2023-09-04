TEST(invalid_key_tests, test_without_value_2)
{
    const string toml{"key = \n", 7};

    const vector<Error> expected{{ 1, 7, "Missing value." },};

    assert_errors(toml, expected);
}
