TEST(invalid_bool_tests, test_just_t)
{
    const string toml{"a = t\n", 6};

    const vector<Error> expected{{ 1, 5, "Invalid value: t" },};

    assert_errors(toml, expected);
}
