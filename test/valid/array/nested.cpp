TEST(valid_array_tests, test_nested)
{
    const string toml("nest = [[\"a\"], [\"b\"]]\n", 22);

    const Table expected = {{ "nest", Value(Array({Value(Array({Value(std::string("a"))})),Value(Array({Value(std::string("b"))}))})) }};

    assert_parsed(toml, expected);
}
