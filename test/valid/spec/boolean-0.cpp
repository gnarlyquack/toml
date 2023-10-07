TEST(valid_spec_tests, test_boolean_0)
{
    const string toml("bool1 = true\nbool2 = false\n", 27);

    const Table expected = {{ std::string("bool1", 5), Value(true) },{ std::string("bool2", 5), Value(false) }};

    assert_parsed(toml, expected);
}
