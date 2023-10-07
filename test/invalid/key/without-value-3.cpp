TEST(invalid_key_tests, test_without_value_3)
{
    const string toml("\"key\"\n", 6);

    const vector<Error> expected = {
        { 5, 1, 6, "Missing '=' between key and value." },
        { 5, 1, 6, "Expected a value." },
    };

    assert_errors(toml, expected);
}
