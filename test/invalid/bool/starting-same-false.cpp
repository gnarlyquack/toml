TEST(invalid_bool_tests, test_starting_same_false)
{
    const string toml{"a = falsey\n", 11};

    const vector<Error> expected{{ 1, 5, "Invalid value: falsey" },};

    assert_errors(toml, expected);
}
