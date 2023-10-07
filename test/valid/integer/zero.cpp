TEST(valid_integer_tests, test_zero)
{
    const string toml("d1 = 0\nd2 = +0\nd3 = -0\n\nh1 = 0x0\nh2 = 0x00\nh3 = 0x00000\n\no1 = 0o0\na2 = 0o00\na3 = 0o00000\n\nb1 = 0b0\nb2 = 0b00\nb3 = 0b00000\n", 122);

    const Table expected = {{ std::string("a2", 2), Value(INT64_C(0)) },{ std::string("a3", 2), Value(INT64_C(0)) },{ std::string("b1", 2), Value(INT64_C(0)) },{ std::string("b2", 2), Value(INT64_C(0)) },{ std::string("b3", 2), Value(INT64_C(0)) },{ std::string("d1", 2), Value(INT64_C(0)) },{ std::string("d2", 2), Value(INT64_C(0)) },{ std::string("d3", 2), Value(INT64_C(0)) },{ std::string("h1", 2), Value(INT64_C(0)) },{ std::string("h2", 2), Value(INT64_C(0)) },{ std::string("h3", 2), Value(INT64_C(0)) },{ std::string("o1", 2), Value(INT64_C(0)) }};

    assert_parsed(toml, expected);
}
