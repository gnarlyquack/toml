TEST(invalid_key_tests, test_escape)
{
    const string toml("\\u00c0 = \"latin capital letter A with grave\"\n", 45);

    const vector<Error> expected = {
        { 0, 1, 1, "Invalid key: \\u00c0" },
    };

    assert_errors(toml, expected);
}
