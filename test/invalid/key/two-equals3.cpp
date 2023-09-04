TEST(invalid_key_tests, test_two_equals3)
{
    const string toml{"a=b=1\n", 6};

    const vector<Error> expected{{ 1, 3, "Invalid value: b=1" },};

    assert_errors(toml, expected);
}
