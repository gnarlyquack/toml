TEST(invalid_key_tests, test_multiline)
{
    const string toml("\"\"\"long\nkey\"\"\" = 1\n", 19);

    // TODO Special case handling invalid use of multiline strings as keys?
    const vector<Error> expected = {
        { 2, 1, 3, "Missing '=' between key and value." },
        { 7, 1, 8, "Unclosed string." },
        { 8, 2, 1, "Invalid key: key\"\"\"" },
    };

    assert_errors(toml, expected);
}
