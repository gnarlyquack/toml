TEST(invalid_key_tests, test_without_value_2)
{
    const string toml("key = \n", 7);

    const vector<Error> expected = {
        { 6, 1, 7, "Expected a value." },
    };

    assert_errors(toml, expected);
}
