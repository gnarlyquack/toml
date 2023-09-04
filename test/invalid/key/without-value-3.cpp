TEST(invalid_key_tests, test_without_value_3)
{
    const string toml{"\"key\"\n", 6};

    const vector<Error> expected{{ 1, 6, "Missing '=' between key and value." },{ 1, 6, "Missing value." },};

    assert_errors(toml, expected);
}
