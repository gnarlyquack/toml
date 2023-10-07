TEST(valid_bool_tests, test_bool)
{
    const string toml("t = true\nf = false\n", 19);

    const Table expected = {{ std::string("f", 1), Value(false) },{ std::string("t", 1), Value(true) }};

    assert_parsed(toml, expected);
}
