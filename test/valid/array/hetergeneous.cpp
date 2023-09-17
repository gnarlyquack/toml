TEST(valid_array_tests, test_hetergeneous)
{
    const string toml("mixed = [[1, 2], [\"a\", \"b\"], [1.1, 2.1]]\n", 41);

    const Table expected = {{ "mixed", Value(Array({Value(Array({Value(INT64_C(1)),Value(INT64_C(2))})),Value(Array({Value(std::string("a")),Value(std::string("b"))})),Value(Array({Value(1.1),Value(2.1)}))})) }};

    assert_parsed(toml, expected);
}
