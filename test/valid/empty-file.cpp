TEST(valid_tests, test_empty_file)
{
    const string toml("", 0);

    const Table expected = {};

    assert_parsed(toml, expected);
}
