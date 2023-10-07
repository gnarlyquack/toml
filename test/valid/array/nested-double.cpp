TEST(valid_array_tests, test_nested_double)
{
    const string toml("nest = [\n\t[\n\t\t[\"a\"],\n\t\t[1, 2, [3]]\n\t]\n]\n", 40);

    const Table expected = {{ std::string("nest", 4), Value(Array({Value(Array({Value(Array({Value(std::string("a", 1))})),Value(Array({Value(INT64_C(1)),Value(INT64_C(2)),Value(Array({Value(INT64_C(3))}))}))}))})) }};

    assert_parsed(toml, expected);
}
