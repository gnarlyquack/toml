TEST(valid_array_tests, test_nested)
{
    const string toml("nest = [[\"a\"], [\"b\"]]\n", 22);

    const Table expected = {{ std::string("nest", 4), Value(Array({Value(Array({Value(std::string("a", 1))})),Value(Array({Value(std::string("b", 1))}))})) }};

    assert_parsed(toml, expected);
}
