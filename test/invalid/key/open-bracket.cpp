TEST(invalid_key_tests, test_open_bracket)
{
    const string toml("[abc = 1\n", 9);

    const vector<Error> expected = {
        { 5, 1, 6, "Expected closing ']' for table header." },
    };

    assert_errors(toml, expected);
}
