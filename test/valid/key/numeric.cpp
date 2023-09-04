TEST(valid_key_tests, test_numeric)
{
    const string toml{"1 = 1\n", 6};

    const Table expected{{ "1", new IntegerValue(1ULL) },};

    assert_parsed(toml, expected);
}
