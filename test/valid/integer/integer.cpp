TEST(valid_integer_tests, test_integer)
{
    const string toml("answer = 42\nposanswer = +42\nneganswer = -42\nzero = 0\n", 53);

    const Table expected = {{ "answer", Value(INT64_C(42)) },{ "neganswer", Value(INT64_C(-42)) },{ "posanswer", Value(INT64_C(42)) },{ "zero", Value(INT64_C(0)) }};

    assert_parsed(toml, expected);
}
