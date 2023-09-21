TEST(invalid_key_tests, test_without_value_1)
{
    const string toml("key\n", 4);

    const vector<Error> expected = {
        { 3, 1, 4, "Missing '=' between key and value." },
        { 3, 1, 4, "Missing value." },
    };

    assert_errors(toml, expected);
}
