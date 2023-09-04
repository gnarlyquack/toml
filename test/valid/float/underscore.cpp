TEST(valid_float_tests, test_underscore)
{
    const string toml{"before = 3_141.5927\nafter = 3141.592_7\nexponent = 3e1_4\n", 56};

    const Table expected{{ "after", new FloatValue(3141.5927) },{ "before", new FloatValue(3141.5927) },{ "exponent", new FloatValue(3.0e14) },};

    assert_parsed(toml, expected);
}
