TEST(valid_float_tests, test_underscore)
{
    const string toml("before = 3_141.5927\nafter = 3141.592_7\nexponent = 3e1_4\n", 56);

    const Table expected = {{ std::string("after", 5), Value(3141.5927) },{ std::string("before", 6), Value(3141.5927) },{ std::string("exponent", 8), Value(3.0e14) }};

    assert_parsed(toml, expected);
}
