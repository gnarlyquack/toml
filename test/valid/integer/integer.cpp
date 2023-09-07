TEST(valid_integer_tests, test_integer)
{
    const string toml{"answer = 42\nposanswer = +42\nneganswer = -42\nzero = 0\n", 53};

    const Table expected{{ "answer", Value::of_integer(42ULL) },{ "neganswer", Value::of_integer(-42ULL) },{ "posanswer", Value::of_integer(42ULL) },{ "zero", Value::of_integer(0ULL) }};

    assert_parsed(toml, expected);
}
