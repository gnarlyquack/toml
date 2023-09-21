TEST(invalid_key_tests, test_space)
{
    const string toml("a b = 1\n", 8);

    const vector<Error> expected = {
        { 2, 1, 3, "Invalid value: b" },
        { 2, 1, 3, "Missing '=' between key and value." },
        { 4, 1, 5, "Expected the end of the line but got: = 1" },
    };

    assert_errors(toml, expected);
}
