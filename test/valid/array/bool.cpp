TEST(valid_array_tests, test_bool)
{
    const string toml("a = [true, false]\n", 18);

    const Table expected = {{ std::string("a", 1), Value(Array({Value(true),Value(false)})) }};

    assert_parsed(toml, expected);
}
