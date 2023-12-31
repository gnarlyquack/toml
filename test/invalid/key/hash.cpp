TEST(invalid_key_tests, test_hash)
{
    const string toml("a# = 1\n", 7);

    const vector<Error> expected = {
        { 1, 1, 2, "Missing '=' between key and value." },
        { 1, 1, 2, "Expected a value." },
    };

    assert_errors(toml, expected);
}
