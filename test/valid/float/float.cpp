TEST(valid_float_tests, test_float)
{
    const string toml("pi = 3.14\npospi = +3.14\nnegpi = -3.14\nzero-intpart = 0.123\n", 59);

    const Table expected = {{ std::string("negpi", 5), Value(-3.14) },{ std::string("pi", 2), Value(3.14) },{ std::string("pospi", 5), Value(3.14) },{ std::string("zero-intpart", 12), Value(0.123) }};

    assert_parsed(toml, expected);
}
