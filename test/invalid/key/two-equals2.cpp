TEST(invalid_key_tests, test_two_equals2)
{
    const string toml{"a==1\n", 5};

    const vector<Error> expected{{ 1, 3, "Invalid value: =1" },};

    assert_errors(toml, expected);
}
