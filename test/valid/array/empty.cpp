TEST(valid_array_tests, test_empty)
{
    const string toml("thevoid = [[[[[]]]]]\n", 21);

    const Table expected = {{ "thevoid", Value(Array({Value(Array({Value(Array({Value(Array({Value(Array({}))}))}))}))})) }};

    assert_parsed(toml, expected);
}
