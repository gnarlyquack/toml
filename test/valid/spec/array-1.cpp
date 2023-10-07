TEST(valid_spec_tests, test_array_1)
{
    const string toml("integers2 = [\n  1, 2, 3\n]\n\nintegers3 = [\n  1,\n  2, # this is ok\n]\n", 66);

    const Table expected = {{ std::string("integers2", 9), Value(Array({Value(INT64_C(1)),Value(INT64_C(2)),Value(INT64_C(3))})) },{ std::string("integers3", 9), Value(Array({Value(INT64_C(1)),Value(INT64_C(2))})) }};

    assert_parsed(toml, expected);
}
