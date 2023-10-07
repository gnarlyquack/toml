TEST(invalid_key_tests, test_without_value_4)
{
    const string toml("\"key\" = \n", 9);

    const vector<Error> expected = {
        { 8, 1, 9, "Expected a value." },
    };

    assert_errors(toml, expected);
}
