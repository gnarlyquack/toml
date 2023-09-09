TEST(valid_float_tests, test_long)
{
    const string toml{"longpi = 3.141592653589793\nneglongpi = -3.141592653589793\n", 58};

    const Table expected{{ "longpi", Value(3.141592653589793) },{ "neglongpi", Value(-3.141592653589793) }};

    assert_parsed(toml, expected);
}
