TEST(invalid_spec_tests, test_key_value_pair_1)
{
    const string toml("key = # INVALID\n", 16);

    const vector<Error> expected = {
        { 6, 1, 7, "Expected a value." },
    };

    assert_errors(toml, expected);
}
