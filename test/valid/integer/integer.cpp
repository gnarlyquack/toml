TEST(valid_integer_tests, test_integer)
{
    const string toml("answer = 42\nposanswer = +42\nneganswer = -42\nzero = 0\n", 53);

    const Table expected = {{ std::string("answer", 6), Value(INT64_C(42)) },{ std::string("neganswer", 9), Value(INT64_C(-42)) },{ std::string("posanswer", 9), Value(INT64_C(42)) },{ std::string("zero", 4), Value(INT64_C(0)) }};

    assert_parsed(toml, expected);
}
