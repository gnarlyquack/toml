TEST(invalid_key_tests, test_after_array)
{
    const string toml("[[agencies]] owner = \"S Cjelli\"\n", 32);

    const vector<Error> expected = {
        { 13, 1, 14, "Expected the end of the line but got: owner = \"S Cjelli\"" },
    };

    assert_errors(toml, expected);
}
