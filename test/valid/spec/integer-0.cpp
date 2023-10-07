TEST(valid_spec_tests, test_integer_0)
{
    const string toml("int1 = +99\nint2 = 42\nint3 = 0\nint4 = -17\n", 41);

    const Table expected = {{ std::string("int1", 4), Value(INT64_C(99)) },{ std::string("int2", 4), Value(INT64_C(42)) },{ std::string("int3", 4), Value(INT64_C(0)) },{ std::string("int4", 4), Value(INT64_C(-17)) }};

    assert_parsed(toml, expected);
}
