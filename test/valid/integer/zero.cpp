TEST(valid_integer_tests, test_zero)
{
    const string toml{"d1 = 0\nd2 = +0\nd3 = -0\n\nh1 = 0x0\nh2 = 0x00\nh3 = 0x00000\n\no1 = 0o0\na2 = 0o00\na3 = 0o00000\n\nb1 = 0b0\nb2 = 0b00\nb3 = 0b00000\n", 122};

    const Table expected{{ "a2", Value(INT64_C(0)) },{ "a3", Value(INT64_C(0)) },{ "b1", Value(INT64_C(0)) },{ "b2", Value(INT64_C(0)) },{ "b3", Value(INT64_C(0)) },{ "d1", Value(INT64_C(0)) },{ "d2", Value(INT64_C(0)) },{ "d3", Value(INT64_C(0)) },{ "h1", Value(INT64_C(0)) },{ "h2", Value(INT64_C(0)) },{ "h3", Value(INT64_C(0)) },{ "o1", Value(INT64_C(0)) }};

    assert_parsed(toml, expected);
}
