TEST(invalid_key_tests, test_bare_invalid_character)
{
    const string toml("bare!key = 123\n", 15);

    const vector<Error> expected = {
        { 0, 1, 1, "Invalid key: bare!key" },
    };

    assert_errors(toml, expected);
}
