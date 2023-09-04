TEST(valid_float_tests, test_float)
{
    const string toml{"pi = 3.14\npospi = +3.14\nnegpi = -3.14\nzero-intpart = 0.123\n", 59};

    const Table expected{{ "negpi", new FloatValue(-3.14) },{ "pi", new FloatValue(3.14) },{ "pospi", new FloatValue(3.14) },{ "zero-intpart", new FloatValue(0.123) },};

    assert_parsed(toml, expected);
}
