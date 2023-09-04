TEST(invalid_key_tests, test_space)
{
    const string toml{"a b = 1\n", 8};

    const vector<Error> expected{
        { 1, 3, "Missing '=' between key and value." },
        { 1, 3, "Invalid value: b = 1" },
    };

    assert_errors(toml, expected);
}
