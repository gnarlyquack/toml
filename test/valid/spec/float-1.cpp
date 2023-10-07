TEST(valid_spec_tests, test_float_1)
{
    const string toml("flt8 = 224_617.445_991_228\n", 27);

    const Table expected = {{ std::string("flt8", 4), Value(224617.445991228) }};

    assert_parsed(toml, expected);
}
