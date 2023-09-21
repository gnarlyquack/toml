TEST(invalid_key_tests, test_start_bracket)
{
    const string toml("[a]\n[xyz = 5\n[b]\n", 17);

    const vector<Error> expected = {
        { 9, 2, 6, "Expected closing ']' for table header." },
    };

    assert_errors(toml, expected);
}
