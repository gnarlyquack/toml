TEST(valid_spec_tests, test_integer_1)
{
    const string toml("int5 = 1_000\nint6 = 5_349_221\nint7 = 53_49_221  # Indian number system grouping\nint8 = 1_2_3_4_5  # VALID but discouraged\n", 122);

    const Table expected = {{ std::string("int5", 4), Value(INT64_C(1000)) },{ std::string("int6", 4), Value(INT64_C(5349221)) },{ std::string("int7", 4), Value(INT64_C(5349221)) },{ std::string("int8", 4), Value(INT64_C(12345)) }};

    assert_parsed(toml, expected);
}
