TEST(valid_float_tests, test_float)
{
    const string toml{"pi = 3.14\npospi = +3.14\nnegpi = -3.14\nzero-intpart = 0.123\n", 59};

    const Table expected{{ "negpi", Value::of_float(-3.14) },{ "pi", Value::of_float(3.14) },{ "pospi", Value::of_float(3.14) },{ "zero-intpart", Value::of_float(0.123) }};

    assert_parsed(toml, expected);
}
